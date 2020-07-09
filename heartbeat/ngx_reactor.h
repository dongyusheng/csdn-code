
#ifndef _NGX_REACTOR_H_INCLUDED_
#define _NGX_REACTOR_H_INCLUDED_

#include "ngx_config.h"
#include "ngx_core.h"

typedef int (*ngx_event_handler_pt)(ngx_event_t *ev);

struct ngx_reactor_s {
	int epfd;
	ngx_event_t *ev;
};

struct ngx_event_s {
	int fd;
	void *arg;
	unsigned int what;  /* READ / WRITE / TIMEOUT and so on */
	
	int status;
	char buffer[BUFFER_LENGTH];
	int length;
	ngx_uint_t last_active;  /* 最近一次发送数据或者接收数据时的时间戳，毫秒 */

	int timer_set;
	int timedout;
	ngx_event_handler_pt handler;

    /* rbtree timer */
	ngx_rbtree_node_t   timer;
};


void ngx_event_set(ngx_event_t *ev, int fd, ngx_event_handler_pt handler, void *arg);
int ngx_event_add(int epfd, unsigned int what, ngx_msec_t timeout, ngx_event_t *ev);
int ngx_event_del(int epfd, ngx_event_t *ev);


int ngx_reactor_init(ngx_reactor_t *reactor);
int ngx_reactor_destory(ngx_reactor_t *reactor);
int ngx_reactor_loop(ngx_reactor_t *reactor);



#endif

