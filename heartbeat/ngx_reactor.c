


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


#include "ngx_config.h"
#include "ngx_core.h"


void 
ngx_event_set(ngx_event_t *ev, int fd, ngx_event_handler_pt handler, void *arg) {

	ev->fd          = fd;
	ev->handler     = handler;
	ev->what        = 0;
	ev->arg         = arg;
	ev->last_active = ngx_current_msec;

	return ;
}


int 
ngx_event_add(int epfd, unsigned int what, ngx_msec_t timeout, ngx_event_t *ev) {

	struct epoll_event ep_ev = {0, {0}};
	ep_ev.data.ptr = ev;
	ev->what |= what;

	if (what & NGX_EVENT_READING )
	    ep_ev.events |= EPOLLIN;
	if (what & NGX_EVENT_WRITING)
        ep_ev.events |= EPOLLOUT;
    if (what & NGX_EVENT_TIMEOUT)
		ngx_event_add_timer(ev, timeout);
	
	int op;
	if (ev->status == 1) {
		op = EPOLL_CTL_MOD;
	} else {
		op = EPOLL_CTL_ADD;
		ev->status = 1;
	}

	if (epoll_ctl(epfd, op, ev->fd, &ep_ev) < 0) {
		printf("event add failed [fd=%d], events[%d]\n", ev->fd, what);
		return -1;
	}

	return 0;
}

int 
ngx_event_del(int epfd, ngx_event_t *ev) {

	struct epoll_event ep_ev = {0, {0}};

	if (ev->status != 1) {
		return -1;
	}

	ep_ev.data.ptr = ev;
	ev->status = 0;
	epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, &ep_ev);

	return 0;
}


int 
ngx_reactor_init(ngx_reactor_t *reactor) {

	if (reactor == NULL) return -1;
	memset(reactor, 0, sizeof(ngx_reactor_t));

	reactor->epfd = epoll_create(1);
	if (reactor->epfd <= 0) {
		printf("create epfd in %s err %s\n", __func__, strerror(errno));
		return -2;
	}

	reactor->ev = (ngx_event_t*)malloc((MAX_EPOLL_EVENTS) * sizeof(ngx_event_t));
	if (reactor->ev == NULL) {
		printf("create epfd in %s err %s\n", __func__, strerror(errno));
		close(reactor->epfd);
		return -3;
	}
}

int 
ngx_reactor_destory(ngx_reactor_t *reactor) {

	close(reactor->epfd);
	free(reactor->ev);

}


int 
ngx_reactor_loop(ngx_reactor_t *reactor) {
	if (reactor == NULL) return -1;
	if (reactor->epfd < 0) return -1;
	if (reactor->ev == NULL) return -1;
	
	struct epoll_event events[MAX_EPOLL_EVENTS+1];
	
	int checkpos = 0, i;
	ngx_msec_t delta = ngx_current_msec;

    /* initial event timer */
	ngx_event_timer_init();

	while (1) {

        // 获取当前时间
        delta = ngx_current_msec;
        ngx_msec_t timer = ngx_event_find_timer();


        // 进行事件轮询
		int nready = epoll_wait(reactor->epfd, events, MAX_EPOLL_EVENTS, timer);
		if (nready < 0) {
			printf("epoll_wait error, exit\n");
			continue;
		}

        // 更新一下当前时间, 也就是更新ngx_current_msec
		ngx_time_update();

		for (i = 0;i < nready;i ++) {
			ngx_event_t *ev = (ngx_event_t*)events[i].data.ptr;
			if ((events[i].events & EPOLLIN) && (ev->what & NGX_EVENT_READING)) {

			    ev->what |= NGX_EVENT_READING;
			}
			if ((events[i].events & EPOLLOUT) && (ev->what & NGX_EVENT_WRITING)) {
				ev->what |= NGX_EVENT_WRITING;
			}

			ev->handler(ev);
		}

        // 将当前时间与epoll_wait()之前的时间进行比较
		delta = ngx_current_msec - delta;
        // 如果超时了, 在函数内部将到时的节点的what表示|上NGX_EVENT_TIMEOUT(超时的)
		if(delta) ngx_event_expire_timers();

	}
}




