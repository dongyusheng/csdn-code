// mtserver.c
// https://blog.csdn.net/qq_41453285/article/details/106882216
// https://github.com/dongyusheng/csdn-code/blob/master/ZeroMQ/mtserver.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <zmq.h>

// 多线程运行函数, 参数为进程的上下文指针
static void *worker_routine(void *arg);

// 从socket套接字上接收消息
static char *s_recv(void *socket);

// 向socket套接字发送消息string
static int s_send(void *socket, char *string);

int main()
{
    int rc;
    
    // 1.创建新的上下文
    void *context = zmq_ctx_new();
    assert(context != NULL);
    
    // 2.创建、绑定代理的前端套接字, 客户端会来连接这个套接字
    void *clients = zmq_socket(context, ZMQ_ROUTER);
    assert(clients != NULL);
    rc = zmq_bind(clients, "tcp://*:5559");
    assert(rc != -1);
    
    // 3.创建、绑定代理后端套接字, 在pthread_create()线程中运行的工人会来连接这个套接字
    void *workers = zmq_socket(context, ZMQ_DEALER);
    assert(workers != NULL);
    rc = zmq_bind(workers, "inproc://workers");
    assert(rc != -1);

    // 4.创建4个服务器(工人), 服务器(工人)连接代理后端, 从ZMQ_DEALER端接收客户端的请求, 然后将响应返回
    int thread_nbr;
    for(thread_nbr = 0; thread_nbr < 4; ++thread_nbr)
    {
        pthread_t worker_pid;
        pthread_create(&worker_pid, NULL, worker_routine, context);
    }

    // 5.启动代理, 前端处理客户端, 后端处理服务器(工人)
    zmq_proxy(clients, workers, NULL);

    // 6.关闭套接字、销毁上下文
    zmq_close(clients);
    zmq_close(workers);
    zmq_ctx_destroy(context);
    
    return 0;
}

static void *worker_routine(void *arg)
{
    // 参数: 进程的上下文指针

    int rc;
    
    // 1.创建套接字、连接代理的ZMQ_DEALER端
    void *receiver = zmq_socket(arg, ZMQ_REP);
    assert(receiver != NULL);
    rc = zmq_connect(receiver, "inproc://workers");

    // 2.循环工作
    while(1)
    {
        // 3.从代理的ZMQ_DEALER端读取客户端的请求
        char *request = s_recv(receiver);
        printf("Received request: %s\n", request);
        free(request);

        // 4.休眠1秒再返回响应
        sleep(1);
        
        // 5.将响应发送给代理, 代理会将响应返回给客户端
        rc = s_send(receiver, "World");
        assert(rc != -1);
    }

    // 6.关闭套接字
    zmq_close(receiver);
    return NULL;
}

static char *s_recv(void *socket)
{
    int size;
    
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    
    size = zmq_msg_recv(&msg, socket, 0);
    if(size == -1)
        return NULL;

    char *string = (char*)malloc(size + 1);
    if(string == NULL)
        return NULL;
    memcpy(string, zmq_msg_data(&msg), size);

    zmq_msg_close(&msg);

    string[size] = 0;

    return string;
}

static int s_send(void *socket, char *string)
{
    int rc;
    
    zmq_msg_t msg;
    zmq_msg_init_size(&msg, strlen(string));

    memcpy(zmq_msg_data(&msg), string, strlen(string));
    
    rc = zmq_msg_send(&msg, socket, 0);
    
    zmq_msg_close(&msg);

    return rc;
}
