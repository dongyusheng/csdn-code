
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
#define KEEPALIVE_INTVAL_PROBES 3
#define SERVER_PORT			8888


static int client_handler(ngx_event_t *ev);
static int recv_cb(int fd, unsigned int what, void *arg);
static int send_cb(int fd, unsigned int what, void *arg);
static int accept_cb(int fd, unsigned int what, void *arg);
static int timeout_cb(int fd, unsigned int what, ngx_msec_t last_active, void *arg);
static int ngx_init_sock(short port);


int recv_cb(int fd, unsigned int what, void *arg) {

	ngx_reactor_t *reactor = (ngx_reactor_t*)arg;
	ngx_event_t *ev = (ngx_event_t*)(reactor->ev + fd);

	int len = recv(fd, ev->buffer, BUFFER_LENGTH, 0);
	ngx_event_del(reactor->epfd, ev);

	if (len > 0) {
		
		ev->length = len;
		ev->buffer[len] = '\0';

        /* it's mandatory to update active clock if connection is still alive */
		ev->last_active = ngx_current_msec;
		
		printf("C[%d]:%s\n", fd, ev->buffer);

		ngx_event_set(ev, fd, client_handler, reactor);
		ngx_event_add(reactor->epfd, NGX_EVENT_READING, KEEPALIVE_INTVAL_MSEC, ev);
		
	} else if (len == 0) {

		close(ev->fd);
		printf("[fd=%d] pos[%ld], closed\n", fd, ev-reactor->ev);
		 
	} else {

		close(ev->fd);
		printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
		
	}

	return len;
}

static int send_cb(int fd, unsigned int what, void *arg) {

	ngx_reactor_t *reactor = (ngx_reactor_t*)arg;
	ngx_event_t *ev = reactor->ev+fd;

	int len = send(fd, ev->buffer, ev->length, 0);
	if (len > 0) {
		printf("send[fd=%d], [%d]%s\n", fd, len, ev->buffer);

		ngx_event_del(reactor->epfd, ev);
		ngx_event_set(ev, fd, client_handler, reactor);
		ngx_event_add(reactor->epfd, NGX_EVENT_WRITING, KEEPALIVE_INTVAL_MSEC, ev);
		
	} else {

		close(ev->fd);

		ngx_event_del(reactor->epfd, ev);
		printf("send[fd=%d] error %s\n", fd, strerror(errno));

	}

	return len;
}

static int accept_cb(int fd, unsigned int what, void *arg) {

	ngx_reactor_t *reactor = (ngx_reactor_t*)arg;
	if (reactor == NULL) return -1;

	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);

	int clientfd;

	if ((clientfd = accept(fd, (struct sockaddr*)&client_addr, &len)) == -1) {
		if (errno != EAGAIN && errno != EINTR) {
			
		}
		printf("accept: %s\n", strerror(errno));
		return -1;
	}

	int i = 0;
	do {
		
		for (i = 0;i < MAX_EPOLL_EVENTS;i ++) {
			if (reactor->ev[i].status == 0) {
				break;
			}
		}
		if (i == MAX_EPOLL_EVENTS) {
			printf("%s: max connect limit[%d]\n", __func__, MAX_EPOLL_EVENTS);
			break;
		}

		int flag = 0;
		if ((flag = fcntl(clientfd, F_SETFL, O_NONBLOCK)) < 0) {
			printf("%s: fcntl nonblocking failed, %d\n", __func__, MAX_EPOLL_EVENTS);
			break;
		}

		ngx_event_set(&reactor->ev[clientfd], clientfd, client_handler, reactor);
		ngx_event_add(reactor->epfd, NGX_EVENT_READING | NGX_EVENT_TIMEOUT, KEEPALIVE_INTVAL_MSEC, &reactor->ev[clientfd]);

	} while (0);

	printf("new connect [%s:%d][time:%ld], pos[%d]\n", 
		inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), reactor->ev[i].last_active, i);

	return 0;

}

static int 
timeout_cb(int fd, unsigned int what, ngx_msec_t last_active, void *arg){

    ngx_msec_t delta = ngx_current_msec - last_active;

	if(delta >= KEEPALIVE_INTVAL_MSEC*KEEPALIVE_INTVAL_PROBES){
        //todo close this connection and other bussiness
        // ...
        close(fd);
	}

    return 0;
}


static int 
client_handler(ngx_event_t *ev){

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
		ngx_event_add(reactor->epfd, NGX_EVENT_READING | NGX_EVENT_TIMEOUT, KEEPALIVE_INTVAL_MSEC, ev);
	}

    return ret;
}


static int 
ngx_reactor_handler(ngx_event_t *ev){

    if (ev->what & NGX_EVENT_READING){
        accept_cb(ev->fd, ev->what, ev->arg);
	}

    return 0;
}


static int 
ngx_reactor_addlistener(ngx_reactor_t *reactor, int sockfd, ngx_event_handler_pt handler) {

	if (reactor == NULL) return -1;
	if (reactor->ev == NULL) return -1;

	ngx_event_set(&reactor->ev[sockfd], sockfd, handler, reactor);
	ngx_event_add(reactor->epfd, NGX_EVENT_READING, 0, &reactor->ev[sockfd]);

	return 0;
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
ngx_init_server_sock(short port) {

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(fd, F_SETFL, O_NONBLOCK);

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

	if (listen(fd, 20) < 0) {
		printf("listen failed : %s\n", strerror(errno));
	}

	return fd;
}


int 
main(int argc, char *argv[]) {

	unsigned short port = SERVER_PORT;
	if (argc == 2) {
		port = atoi(argv[1]);
	}

    /* it's mandatory to update clock */
    ngx_time_update();

	int sockfd = ngx_init_server_sock(port);

	ngx_reactor_t *reactor = (ngx_reactor_t*)malloc(sizeof(ngx_reactor_t));
	ngx_reactor_init(reactor);
	
	ngx_reactor_addlistener(reactor, sockfd, ngx_reactor_handler);
	ngx_reactor_loop(reactor);

	ngx_reactor_destory(reactor);
	close(sockfd);

	return 0;
}



