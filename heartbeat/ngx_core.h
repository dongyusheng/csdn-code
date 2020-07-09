
#ifndef _NGX_CORE_H_INCLUDED_
#define _NGX_CORE_H_INCLUDED_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>


#define NGX_EVENT_READING	0x01	/**< error encountered while reading */
#define NGX_EVENT_WRITING	0x02	/**< error encountered while writing */
#define NGX_EVENT_EOF		0x10	/**< eof file reached */
#define NGX_EVENT_ERROR		0x20	/**< unrecoverable error encountered */
#define NGX_EVENT_TIMEOUT	0x40	/**< user-specified timeout reached */
#define NGX_EVENT_CONNECTED	0x80	/**< connect operation finished. */

#define BUFFER_LENGTH		4096
#define MAX_EPOLL_EVENTS	1024


typedef struct ngx_event_s           ngx_event_t;
typedef struct ngx_reactor_s         ngx_reactor_t;


# ifndef offsetof
#  define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
# endif


#define ngx_abs(value)       (((value) >= 0) ? (value) : - (value))
#define ngx_max(val1, val2)  ((val1 < val2) ? (val2) : (val1))
#define ngx_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))


#define  NGX_OK          0
#define  NGX_ERROR      -1
#define  NGX_AGAIN      -2


#include "ngx_rbtree.h"
#include "ngx_reactor.h"
#include "ngx_event_timer.h"
#include "ngx_times.h"
#include "ngx_message.h"
#include "ngx_binary_protocol.h"


#endif


