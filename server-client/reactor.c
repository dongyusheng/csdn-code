// https://blog.csdn.net/qq_41453285/article/details/106976702
// https://github.com/dongyusheng/csdn-code/blob/master/server-client/reactor.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>


#define BUFFER_LENGTH		4096
#define MAX_EPOLL_EVENTS	1024
#define SERVER_PORT         8888

typedef int NCALLBACK(int ,int, void*);

// 每一个IO封装为一个事件
struct ntyevent {
	int fd;       //IO对应的fd
	int events;   //IO对应的事件, 可读可写, 添加这个成员是为了与epoll_wait()作分离
	void *arg;    //回调函数对应的参数, 是struct ntyreactor
	int (*callback)(int fd, int events, void *arg);//回调函数
	
	int status;                  //表示当前状态，1代表当前已经在reactor堆中，0表示不在
	char buffer[BUFFER_LENGTH]; //recv()、send()缓冲区
	int length;                 //缓冲区当前数据的长度
	long last_active;
};

// reactor主体
struct ntyreactor {
	int epfd;                //epoll的fd
	struct ntyevent *events; //事件集合, 可以用红黑树等数据结构存储, 此处我们简单用链表存储
};


int recv_cb(int fd, int events, void *arg);
int send_cb(int fd, int events, void *arg);


/**********************************************/
//下面这三个函数是在针对于reactor中的epoll事件表操作的

// 将一个IO封装为一个ntyevent事件结构
int nty_event_set(struct ntyevent *ev, int fd, NCALLBACK callback, void *arg) {
	ev->fd = fd;
	ev->callback = callback;
	ev->events = 0;
	ev->arg = arg;
	ev->last_active = time(NULL);
	
	return 0;
}

// 将一个事件添加/更新到reactor的epoll的事件表中
int nty_event_add(int epfd, int events, struct ntyevent *ev) {

	struct epoll_event ep_ev = {0, {0}};
	ep_ev.data.ptr = ev; //epoll_wait()返回的时候可以通过这个成员获取整个ntyevent结构
	ep_ev.events = ev->events = events;

	int op;
	if (ev->status == 1) {  //如果该事件已经在reactor中了, 那么就更新
		op = EPOLL_CTL_MOD;
	} else {
		op = EPOLL_CTL_ADD; //否则就新加入
		ev->status = 1;
	}

	if (epoll_ctl(epfd, op, ev->fd, &ep_ev) < 0) {
		printf("event add failed [fd=%d], events[%d]\n", ev->fd, events);
		return -1;
	}

	return 0;
}

// 将一个事件移出reactor的epoll的事件表中
// 为什么将事件加入到epoll事件表中之后还要删除呢？有几个原因：
//     1.close()  -> 事件已经关闭了, 移出
//     2.read()/write()之间切换的时候, 需要移出再添加
//     3.当一个事件有事件触发之后, 如果是LE模式, 那么会一直触发, 但是我们不想现在处理, 因此我们需要先将其移出, 等到适合的时候再将其添加到epoll事件表中
//       （例如，我们想从该事件中读取指定大小的数据, 但是每次只能读取一部分, 因此每读取一部分就移出, 然后再添加进去读取, 等全部读取完成之后进行触发处理）
int nty_event_del(int epfd, struct ntyevent *ev) {

	struct epoll_event ep_ev = {0, {0}};

	// 不在里面就退出
	if (ev->status != 1) {
		return -1;
	}

	// 删除
	ep_ev.data.ptr = ev;
	ev->status = 0;
	epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, &ep_ev);

	return 0;
}



// 事件的回调函数: 接收数据
int recv_cb(int fd, int events, void *arg) {
	// 接收数据的时候, 如果一次没有接收完, 那么就继续调用recv()接收数据

	struct ntyreactor *reactor = (struct ntyreactor*)arg;
	struct ntyevent *ev = reactor->events + fd;

	int len = recv(fd, ev->buffer, BUFFER_LENGTH, 0);
	nty_event_del(reactor->epfd, ev);

	if (len > 0) {
		
		ev->length = len;
		ev->buffer[len] = '\0';

		printf("C[%d]:%s\n", fd, ev->buffer);

		nty_event_set(ev, fd, send_cb, reactor);
		nty_event_add(reactor->epfd, EPOLLOUT, ev);
		
		
	} else if (len == 0) { //如果对方关闭了

		close(ev->fd);
		printf("[fd=%d] pos[%ld], closed\n", fd, ev-reactor->events);
		 
	} else {
		//if(errno == EAGAIN || errno == EWOULDBLOCK)
		//	continue;
		return 0;
		
		close(ev->fd);
		printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
		
	}

	return len;
}

// 事件的回调函数: 
int send_cb(int fd, int events, void *arg) {

	struct ntyreactor *reactor = (struct ntyreactor*)arg;
	struct ntyevent *ev = reactor->events + fd;

	int len = send(fd, ev->buffer, ev->length, 0);
	if (len > 0) {
		printf("send[fd=%d], [%d]%s\n", fd, len, ev->buffer);

		nty_event_del(reactor->epfd, ev);
		nty_event_set(ev, fd, recv_cb, reactor);
		nty_event_add(reactor->epfd, EPOLLIN, ev);
		
	} else {

		close(ev->fd);

		nty_event_del(reactor->epfd, ev);
		printf("send[fd=%d] error %s\n", fd, strerror(errno));

	}

	return len;
}

// 事件的回调函数: TcpServer的回调函数, 用来接收客户端的连接
int accept_cb(int fd, int events, void *arg) {

	struct ntyreactor *reactor = (struct ntyreactor*)arg;
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
			if (reactor->events[i].status == 0) {
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

		// 客户端第一次连接时, 我们设置其回调函数为recv_cb(), 因为一般先是客户端给服务器发送数据, 然后服务器再给客户端回送数据
		// 根据自己的业务场景
		nty_event_set(&reactor->events[clientfd], clientfd, recv_cb, reactor);
		nty_event_add(reactor->epfd, EPOLLIN, &reactor->events[clientfd]);

	} while (0);

	printf("new connect [%s:%d][time:%ld], pos[%d]\n", 
		inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), reactor->events[i].last_active, i);

	return 0;
}


// 初始化一个Tcp Server
int init_sock(short port) {

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

// reactor初始化
int ntyreactor_init(struct ntyreactor *reactor) {

	if (reactor == NULL) return -1;
	memset(reactor, 0, sizeof(struct ntyreactor));

	reactor->epfd = epoll_create(1);
	if (reactor->epfd <= 0) {
		printf("create epfd in %s err %s\n", __func__, strerror(errno));
		return -2;
	}

	reactor->events = (struct ntyevent*)malloc((MAX_EPOLL_EVENTS) * sizeof(struct ntyevent));
	if (reactor->events == NULL) {
		printf("create epfd in %s err %s\n", __func__, strerror(errno));
		close(reactor->epfd);
		return -3;
	}
	
	return 0;
}

// reactor销毁
int ntyreactor_destroy(struct ntyreactor *reactor) {
	close(reactor->epfd);
	free(reactor->events);
}


// 将一个事件加入到reactor中
int ntyreactor_addlistener(struct ntyreactor *reactor, int sockfd, NCALLBACK *acceptor) {

	if (reactor == NULL) return -1;
	if (reactor->events == NULL) return -1;

	nty_event_set(&reactor->events[sockfd], sockfd, acceptor, reactor);
	nty_event_add(reactor->epfd, EPOLLIN, &reactor->events[sockfd]);

	return 0;
}


// reactor运行函数
int ntyreactor_run(struct ntyreactor *reactor) {
	if (reactor == NULL) return -1;
	if (reactor->epfd < 0) return -1;
	if (reactor->events == NULL) return -1;
	
	struct epoll_event events[MAX_EPOLL_EVENTS+1];
	
	int checkpos = 0, i;

	while (1) {

		// 超时
		long now = time(NULL);
		for (i = 0;i < 100;i ++, checkpos ++) {
			if (checkpos == MAX_EPOLL_EVENTS) {
				checkpos = 0;
			}

			if (reactor->events[checkpos].status != 1) {
				continue;
			}

			long duration = now - reactor->events[checkpos].last_active;

			if (duration >= 60) {
				close(reactor->events[checkpos].fd);
				printf("[fd=%d] timeout\n", reactor->events[checkpos].fd);
				nty_event_del(reactor->epfd, &reactor->events[checkpos]);
			}
		}


		int nready = epoll_wait(reactor->epfd, events, MAX_EPOLL_EVENTS, 1000);
		if (nready < 0) {
			printf("epoll_wait error, exit\n");
			continue;
		}

		for (i = 0;i < nready;i ++) {
			//获取事件结构
			struct ntyevent *ev = (struct ntyevent*)events[i].data.ptr;

			// 此处判断一个条件也可以, 此处我们判断两次, 为了保险
			if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN)) {
				ev->callback(ev->fd, events[i].events, ev->arg);
			}
			if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)) {
				ev->callback(ev->fd, events[i].events, ev->arg);
			}
		}
	}
}


int main(int argc, char *argv[]) {

	unsigned short port = SERVER_PORT;
	if (argc == 2) {
		port = atoi(argv[1]);
	}

	int sockfd = init_sock(port);

	struct ntyreactor *reactor = (struct ntyreactor*)malloc(sizeof(struct ntyreactor));
	ntyreactor_init(reactor);
	
	ntyreactor_addlistener(reactor, sockfd, accept_cb);
	
	ntyreactor_run(reactor);

	ntyreactor_destroy(reactor);
	close(sockfd);
	
	return 0;
}
