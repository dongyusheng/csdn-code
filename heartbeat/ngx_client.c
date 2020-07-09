
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


#include "ngx_config.h"
#include "ngx_core.h"

#define KEEPALIVE_INTVAL_MSEC 1000*5
#define SERVER_PORT			8888

static int recv_cb(int fd, int events, void *arg);
static int send_cb(int fd, int events, void *arg);
static int handler(ngx_event_t *ev);
static int ngx_init_client_sock(char* ip, char* port);
static int timeout_cb(int fd, unsigned int what, ngx_msec_t last_active, void *arg);


static int 
recv_cb(int fd, int events, void *arg) {

	ngx_reactor_t *reactor = (ngx_reactor_t*)arg;
	ngx_event_t *ev = (ngx_event_t*)(reactor->ev+fd);

	int len = recv(fd, ev->buffer, BUFFER_LENGTH, 0);
	ngx_event_del(reactor->epfd, ev);

	if (len > 0) {
		
		ev->length = len;
		ev->buffer[len] = '\0';

		printf("C[%d]:%s\n", fd, ev->buffer);

        /* it's mandatory to update active clock if connection is still alive */
		ev->last_active = ngx_current_msec;

		/* todo : send heartbeat response */

		ngx_event_set(ev, fd, handler, reactor);
		ngx_event_add(reactor->epfd, NGX_EVENT_WRITING | NGX_EVENT_TIMEOUT, KEEPALIVE_INTVAL_MSEC, ev);
		
	} else if (len == 0) {

		close(ev->fd);
		printf("[fd=%d] pos[%ld], closed\n", fd, ev-reactor->ev);
		 
	} else {

		close(ev->fd);
		printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
		
	}

	return len;
}

static int 
send_cb(int fd, int events, void *arg) {

	ngx_reactor_t *reactor = (ngx_reactor_t*)arg;
	ngx_event_t *ev = reactor->ev+fd;

	int len = send(fd, ev->buffer, ev->length, 0);
	if (len > 0) {
		printf("send[fd=%d], [%d]%s\n", fd, len, ev->buffer);

		ngx_event_del(reactor->epfd, ev);
		ngx_event_set(ev, fd, handler, reactor);
		ngx_event_add(reactor->epfd, NGX_EVENT_READING | NGX_EVENT_TIMEOUT, KEEPALIVE_INTVAL_MSEC, ev);
		
	} else {

		close(ev->fd);

		ngx_event_del(reactor->epfd, ev);
		printf("send[fd=%d] error %s\n", fd, strerror(errno));

	}

	return len;
}

static int 
timeout_cb(int fd, unsigned int what, ngx_msec_t last_active, void *arg){

    printf("ev: %d current: %ld, last_active:%ld\n", fd, ngx_current_msec, last_active);
	
    ngx_msec_t delta = ngx_current_msec - last_active;
	if (delta >= KEEPALIVE_INTVAL_MSEC){
        //todo send heartbeat
        printf("ev: %d timeout: %ld\n", fd, delta);
	}

    return 0;
}


static int 
handler(ngx_event_t *ev){

    int ret = 0;	
	
    if (ev->what & NGX_EVENT_READING){
        ret = recv_cb(ev->fd, ev->what, ev->arg);
	}

	if (ev->what & NGX_EVENT_WRITING){
        ret = send_cb(ev->fd, ev->what, ev->arg);
	}

	if (ev->what & NGX_EVENT_TIMEOUT){
        ret = timeout_cb(ev->fd, ev->what, ev->last_active, ev->arg);

	    /* readd timer for event */
		ngx_reactor_t *reactor = (ngx_reactor_t*)ev->arg;
		if (reactor == NULL) return -1;
		ev->timer_set = 0;
		ngx_event_add(reactor->epfd, NGX_EVENT_READING | NGX_EVENT_TIMEOUT, KEEPALIVE_INTVAL_MSEC, ev);
	}

    return ret;
}

static int 
set_socket_non_block(int fd)
{
    int flags, res;

    flags = fcntl(fd, F_GETFL);
    if (flags == -1)
    {
        perror("error : cannot get socket flags!\n");
        exit(1);
    }

    flags |= O_NONBLOCK;
    res    = fcntl(fd, F_SETFL, flags);
    if (res == -1)
    {
        perror("error : cannot set socket flags!\n");
        exit(1);
    }

    return 0;
}


static int 
ngx_init_client_sock(char* ip, char* port) {

	struct addrinfo hint, *result;
    int res, sfd;
    char buf[20];

    memset(&hint, 0, sizeof(hint));
    hint.ai_family   = AF_INET;
    hint.ai_socktype = SOCK_STREAM;

    res = getaddrinfo(ip, port, &hint, &result);
    if (res == -1)
    {
        perror("error : cannot get socket address!\n");
        return -1;
    }

    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd == -1)
    {
        perror("error : cannot get socket file descriptor!\n");
        return -1;
    }

    res = connect(sfd, result->ai_addr, result->ai_addrlen);
    if (res == -1)
    {
        perror("error : cannot connect the server!\n");
        return -1;
    }

    set_socket_non_block(sfd);
	
	return sfd;
}



int 
main(int argc, char *argv[]) {

    if(argc != 3){
        printf("usage : ngx_client ip port.\n");
		return 0;
	}

	/* it's mandatory to update clock at startup phase */
    ngx_time_update();

	int sockfd = ngx_init_client_sock(argv[1], argv[2]);
	if(sockfd < 0){
        printf("init client socket failed.\n");
	}

	ngx_reactor_t *reactor = (ngx_reactor_t*)malloc(sizeof(ngx_reactor_t));
	ngx_reactor_init(reactor);	
	ngx_event_set(&reactor->ev[sockfd], sockfd, handler, reactor);
	ngx_event_add(reactor->epfd, NGX_EVENT_READING | NGX_EVENT_TIMEOUT, KEEPALIVE_INTVAL_MSEC, &reactor->ev[sockfd]);
	ngx_reactor_loop(reactor);

	ngx_reactor_destory(reactor);
	close(sockfd);

	return 0;
}



