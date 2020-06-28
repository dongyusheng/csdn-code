// reactor.c
// https://blog.csdn.net/qq_41453285/article/details/103001772
// https://blog.csdn.net/qq_41453285/article/details/106976702
// https://github.com/dongyusheng/csdn-code/blob/master/server-client/reactor.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <errno.h>
#include <time.h>
#include <libgen.h>
#include <fcntl.h>

#define MAX_EPOLL_EVENTS    1024
#define MAX_BUFFER_SIZE     4096

typedef int NCALLBACK(int, int, void*);


// 事件结构体, 每个套接字都会被封装为一个事件
struct  ntyevent {
    int fd;           // 事件对应的fd
    int events;       // 事件类型（  本代码中我们只处理EPOLL_IN和EPOLL_OUT）

    void *arg;        // 事件回调函数的参数3, 实际传入的是一个struct ntyreactor结构体指针
    int (*callback)(int fd, int events, void *arg); //事件回调函数

    int status;       // 当前事件是否位于epoll集合中: 1表示在, 0表示不在

    char buffer[MAX_BUFFER_SIZE]; // 读写缓冲区
    int length;       //缓冲区数据的长度
    
    long last_active; // 最后一次活跃的时间
};


// Reactor主体
struct ntyreactor {
    int epoll_fd;             // epoll套接字
    struct ntyevent *events; // reactor当前处理的事件集
};

// 创建一个Tcp Server
int init_server(char *ip, short port);
// 向reactor中添加一个服务器监听事件
int ntyreactor_addlistener(struct ntyreactor *reactor, int fd, NCALLBACK callback);


/***下面这3个函数是用来对reactor操作的***/
// 初始化reactor
struct ntyreactor *ntyreactor_init();
// 销毁reactor
int ntyreactor_destroy(struct ntyreactor *reactor);
// reactor运行函数
int ntyreactor_run(struct ntyreactor *reactor);



/***下面这3个函数是用来对ntyevent事件结构操作的***/
// 将一个fd封装到事件结构中
int nty_event_set(struct ntyevent *ev, int fd, int event, int length, int status, NCALLBACK callback, void *arg);
// 将一个事件添加/更新到epoll的事件表中
int nty_event_add(int epoll_fd, struct ntyevent* ev);
// 将一个事件移出epoll事件表
int nty_event_del(int epoll_fd, struct ntyevent* event);


/***下面这3个函数是ntyevent事件可以使用的回调函数***/
int accept_callback(int fd, int events, void *arg);
int recv_callback(int fd, int events, void *arg);
int send_callback(int fd, int events, void *arg);



int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("usage: ./%s [ip] [port]\n", basename(argv[0]));
        exit(EXIT_FAILURE);
    }

    char *ip = argv[1];
    short port = atoi(argv[2]);
    
    int sock_fd;

    // 1.初始化一个Tcp Server
    sock_fd = init_server(ip, port);

    // 2.初始化reactor
    struct ntyreactor *reactor = ntyreactor_init();
    if( reactor == NULL)
    {
        printf("Error in %s(), ntyreactor_init: create reactor error\n", __func__);
        exit(EXIT_FAILURE);
    }

    // 3.将Tcp Server添加到reactor事件集中
    ntyreactor_addlistener(reactor, sock_fd, accept_callback);

    // 4.运行reactor
    ntyreactor_run(reactor);

    // 5.销毁
    ntyreactor_destroy(reactor);

    close(sock_fd);
    
    return 0;
}

int init_server(char *ip, short port)
{
    // 1.创建套接字
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1)
    {
        printf("Error in %s(), socket: %s\n", __func__, strerror(errno));
        return -1;
    }

    // 2.初始化服务器地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, ip, (void*)&server_addr.sin_addr.s_addr) == -1)
    {
        printf("Error in %s(), inet_pton: %s\n", __func__, strerror(errno));
        return -1;
    }
    server_addr.sin_port = htons(port);

    // 3.绑定服务器地址
    if(bind(sock_fd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        printf("Error in %s(), bind: %s\n", __func__, strerror(errno));
        return -1;
    }

    // 3.监听
    if(listen(sock_fd, 20) == -1)
    {
        printf("Error in %s(), listen: %s\n", __func__, strerror(errno));
        return -1;
    }

    printf("Listen start [%s:%d]...\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    
    return sock_fd;
}

struct ntyreactor *ntyreactor_init()
{
    // 1.创建一个reactor
    struct ntyreactor *reactor = (struct ntyreactor*)malloc(sizeof(struct ntyreactor));
    if(reactor == NULL)
        return NULL;
    memset(reactor, 0, sizeof(struct ntyreactor));

    // 2.创建reacotr的epoll_fd
    reactor->epoll_fd = epoll_create(1);
    if(reactor->epoll_fd == -1)
    {
        printf("Error in %s(), epoll_create: %s\n", __func__, strerror(errno));
        free(reactor);
        return NULL;
    }

    // 3.创建reactor的事件集
    reactor->events = (struct ntyevent*)malloc(sizeof(struct ntyevent) * MAX_EPOLL_EVENTS);
    if(reactor->events == NULL)
    {
        printf("Error in %s(), malloc: %s\n", __func__, strerror(errno));
        close(reactor->epoll_fd);
        free(reactor);
        return NULL;
    }

    return reactor;
}

int ntyreactor_destroy(struct ntyreactor *reactor)
{
    if(reactor == NULL)
    {
        printf("Error in %s(): %s\n", __func__, "reactor arg is NULL");
        return -1;
    }

    // 关闭epoll_fd、销毁事件集、释放结构
    close(reactor->epoll_fd);
    free(reactor->events);

    free(reactor);
    
    return 0;
}

int ntyreactor_run(struct ntyreactor *reactor)
{
    // 1.判断参数
    if(reactor == NULL || reactor->epoll_fd < 0 || reactor->events == NULL)
    {
        printf("Error in %s(): %s\n", __func__, "reactor arg is error");
        return -1;
    }


    struct epoll_event ep_events[MAX_EPOLL_EVENTS + 1];

    // 2.进行epoll_wait()
    int nready;
    while(1)
    {
        // 超时检测
        /*
        int checkpos = 0, i;
        long now = time(NULL);
		for (i = 0; i < MAX_EPOLL_EVENTS; i++, checkpos ++) {
			if (checkpos == MAX_EPOLL_EVENTS) {
				checkpos = 0;
			}
            // 如果当前索引处的事件status为0, 则不检测, 进行下一个
			if (reactor->events[checkpos].status != 1) {
				continue;
			}

            // 如果超过60秒, 那么就认定为超时, 超时后关闭移除
			long duration = now - reactor->events[checkpos].last_active;
			if (duration >= 60) {
				close(reactor->events[checkpos].fd);
				printf("[fd=%d] timeout\n", reactor->events[checkpos].fd);
				nty_event_del(reactor->epfd, &reactor->events[checkpos]);
			}
		}*/
        
        nready = epoll_wait(reactor->epoll_fd, ep_events, MAX_EPOLL_EVENTS, 1000);
        // 3.函数出错
        if(nready == -1)
        {
            // 如果函数在阻塞过程中接收到信号, 那么继续进行epoll_wait()
            if(errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
            printf("Error in %s(), epoll_wait: %s\n", __func__, strerror(errno));
            return -1;
        }
        // 4.函数超时
        else if(nready == 0)
            continue;
        // 5.有事件准备好
        else
        {
            // 遍历处理已就绪的事件
            int i;
            for(i = 0; i < nready; ++i)
            {
                // 获取事件结构体, 保存在struct epoll_event结构的data.ptr中
                struct ntyevent* ev = (struct ntyevent*)ep_events[i].data.ptr;

                // 如果事件可读
                if((ep_events[i].events & EPOLLIN) && (ev->events & EPOLLIN))
                    ev->callback(ev->fd, ev->events, ev->arg);

                // 如果事件可写
                if((ep_events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT))
                    ev->callback(ev->fd, ev->events, ev->arg);
            }
        }
    }

    return 0;
}

int ntyreactor_addlistener(struct ntyreactor *reactor, int fd, NCALLBACK callback)
{
    if(reactor == NULL || fd <0 || callback == NULL)
    {
        printf("Error in %s(): %s\n", __func__, "arg error");
        return -1;
    }

    // 初始化ntyevent事件结构, 然后添加到reactor的epoll事件表中即可
    nty_event_set(&reactor->events[fd], fd, EPOLLIN, 0, 0, callback, reactor);
    nty_event_add(reactor->epoll_fd, &reactor->events[fd]);

    return 0;
}

int nty_event_set(struct ntyevent *ev, int fd, int event, int length, int status, NCALLBACK callback, void *arg)
{
    if(ev == NULL || fd <0 || event <0 || length < 0 || callback == NULL || arg == NULL || status < 0)
    {
        printf("Error in %s(): %s\n", __func__, "arg error");
        return -1;
    }

    // 初始化ntyevent结构的相关内容即可
    ev->fd = fd;
    ev->events = event;
    ev->arg = arg;
    ev->callback = callback;
    ev->status = status;
    ev->length = length;
    ev->last_active = time(NULL);

    return 0;
}

int nty_event_add(int epoll_fd, struct ntyevent* ev)
{
    if(epoll_fd <0 || ev == NULL)
    {
        printf("Error in %s(): %s\n", __func__, "arg error");
        return -1;
    }
    
    // 1.创建一个epoll事件结构
    struct epoll_event ep_event;
    memset(&ep_event, 0, sizeof(ep_event));
    ep_event.events = ev->events;
    ep_event.data.ptr = ev;
    //ep_event.data.fd = ev->fd; data成员是一个联合体, 不能同时使用fd和ptr成员

    // 2.如果当前ev已经在epoll事件表中, 那么就修改; 否则就把ev加入到epoll事件表中
    int op;
    if(ev->status == 0)
    {
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    } 
    else
        op = EPOLL_CTL_MOD;

    // 3.添加/更新 
    if(epoll_ctl(epoll_fd, op, ev->fd, &ep_event) == -1)
    {
        printf("Error in %s(), epoll_ctl: %s\n", __func__, strerror(errno));
        return -1;
    }

    return 0;
}

int nty_event_del(int epoll_fd, struct ntyevent* ev)
{
    if(epoll_fd < 0 || ev == NULL || ev->status != 1)
    {
        printf("Error in %s(): %s\n", __func__, "ev arg is error");
        return -1;
    }

    // 初始要删除的epoll事件结构
    struct epoll_event ep_event;
    memset(&ep_event, 0, sizeof(ep_event));
    ep_event.data.ptr = ev;
    //ep_event.data.fd = ev->fd; data成员是一个枚举, 不能同时使用ptr和fd成员
    ev->status = 0;

    // 从epoll事件表中删除epoll事件
    if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ev->fd, &ep_event) == -1)
    {
        printf("Error in %s(), epoll_ctl: %s\n", __func__, strerror(errno));
        return -1;
    }
    
    return 0;
}

int accept_callback(int fd, int events, void *arg)
{
    // 1.获得reactor结构
    struct ntyreactor *reactor = (struct ntyreactor*)arg;
    // 2.获取该fd对应的事件结构
    struct ntyevent *ev = reactor->events + fd;

    // 3.初始化客户端地址结构
    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0 , sizeof(cli_addr));
    socklen_t len = sizeof(cli_addr);

    // 4.接收客户端
    int cli_fd;
    cli_fd = accept(ev->fd, (struct sockaddr*)&cli_addr, &len);
    if(cli_fd == -1)
    {
        printf("Error in %s(), accept: %s\n", __func__, strerror(errno));
        return -1;
    }

    int i;
    do {
        // 5.在reactor事件表中找到第一个空位置, 用i表示新事件存放的位置, 也是其套接字的值
        // reactor->events的0、1、2、3、4都被占用了, 客户端第一个可以使用的套接字为5, 因此此处从5开始遍历
        for(i = 5; i< MAX_EPOLL_EVENTS; ++i)
        {
            if(reactor->events[i].status == 0)
                break;
        }

        // 6.如果满了, 就退出
        if(i == MAX_EPOLL_EVENTS)
        {
            printf("Error in %s(): max connect limit[%d]\n", __func__, MAX_EPOLL_EVENTS);
            return -1;
        }

        // 7.将套接字设置为非阻塞
        int flag = 0;
		if ((flag = fcntl(cli_fd, F_SETFL, O_NONBLOCK)) < 0) {
            printf("Error in %s(), fcntl: %s\n", __func__, strerror(errno));
			return -1;
		}
        
        // 8.将新事件添加到reactor事件表中
        // 此处我们将新客户端的回调函数首先设置为recv_callback, 事件类型为EPOLLIN, 因为一般都是客户端向服务器发送数据的
        nty_event_set(&reactor->events[cli_fd], cli_fd, EPOLLIN, 0, 0, recv_callback, reactor);
        nty_event_add(reactor->epoll_fd, &reactor->events[cli_fd]);
    } while(0);

    printf("New connect: [%s:%d], [time:%ld], pos[%d]\n", \
        inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), reactor->events[cli_fd].last_active, i);

    return 0;
}

int recv_callback(int fd, int events, void *arg)
{
    // 1.获得reactor结构
    struct ntyreactor *reactor =(struct ntyreactor*)arg;
    // 2.获取该fd对应的事件结构
    struct ntyevent *ev = reactor->events + fd;

    // 3.先将事件从epoll事件集移除
    nty_event_del(reactor->epoll_fd, ev);
    
    // 3.接收数据
    int rc = recv(ev->fd, ev->buffer, MAX_BUFFER_SIZE, 0);
    if(rc < 0)        //recv出错
    {
        //if(errno == EAGAIN || errno == EWOULDBLOCK)
        //    return rc;
        
        printf("Error in %s(), recv: %s\n", __func__, strerror(errno));

        // 此处我们不再需要将该nty_event从epoll事件集移除, 因为上面我们已经移除了
        close(ev->fd);
    }
    else if(rc == 0)  //对方关闭了
    {
        printf("Client closed the connection, fd = %d\n", ev->fd);

        // 此处我们也当做错误处理
        // 此处我们不再需要将该nty_event从epoll事件集移除, 因为上面我们已经移除了
        close(ev->fd);
    } 
    else              //接收到数据
    {
        ev->buffer[rc] = '\0';
        printf("Recv[fd = %d]: %s\n", ev->fd, ev->buffer);

        // 将事件变为可读, 然后加入到epoll事件表中
        nty_event_set(ev, ev->fd, EPOLLOUT, rc, 0, send_callback, reactor);
        nty_event_add(reactor->epoll_fd, ev);
    }

    return rc;
}

int send_callback(int fd, int events, void *arg)
{
    // 1.获得reactor结构
    struct ntyreactor *reactor =(struct ntyreactor*)arg;
    // 2.获取该fd对应的事件结构
    struct ntyevent *ev = reactor->events + fd;

    // 3.此处我们把接收的内容再回送给对象, 因此使用的是ev->buffer
    int rc = send(ev->fd, ev->buffer, ev->length, 0);
    if(rc > 0) //send成功
    {
        printf("Send[fd = %d]: %s\n", ev->fd, ev->buffer);

        // 移除、添加: 将其变为可读
        nty_event_del(reactor->epoll_fd, ev);
        nty_event_set(ev, ev->fd, EPOLLIN, 0, 0, recv_callback, reactor);
        nty_event_add(reactor->epoll_fd, ev);
    }
    else //send失败
    {
        printf("Error in %s(), send: %s\n", __func__, strerror(errno));

        // 关闭、移除
        close(ev->fd);
        nty_event_del(reactor->epoll_fd, ev);
    }

    return rc;
}
