// mtrelay.c
// https://blog.csdn.net/qq_41453285/article/details/106946623
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <zmq.h>

// PAIR2套接字执行函数, 参数为主进程的上下文对象指针
static void *step2(void *arg);

// PAIR1套接字执行函数, 参数为主进程的上下文对象指针
static void *step1(void *arg);

// 从socket套接字上接收消息
static char *s_recv(void *socket);
 
// 向socket套接字发送消息string
static int s_send(void *socket, char *string);

int main()
{
    int rc;
    
    // 1.创建新的上下文对象
    void *context = zmq_ctx_new();
    assert(context != NULL);

    // 2.创建、绑定PAIR3套接字, PAIR2会连接该套接字并向该套接字发送消息
    void *receiver = zmq_socket(context, ZMQ_PAIR);
    assert(receiver != NULL);
    rc = zmq_bind(receiver, "inproc://step3");
    assert(rc != -1);

    // 3.创建线程, 线程中创建PAIR2套接字, PAIR2套接字会向PAIR3发送消息
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, step2, context);

    // 4.阻塞等待接收PAIR2发来消息
    char *string = s_recv(receiver);
    assert(string != NULL);
    free(string);

    // 5.打印消息
    printf("Test successful!\n");

    // 6.关闭套接字、销毁上下文
    zmq_close(receiver);
    zmq_ctx_destroy(context);
    
    return 0;
}

static void *step2(void *arg)
{
    // 参数为上下文对象指针

    int rc;
    
    // 1.创建、绑定PAIR2套接字, PAIR1会连接该套接字并向该套接字发送消息
    void *receiver = zmq_socket(arg, ZMQ_PAIR);
    assert(receiver != NULL);
    rc = zmq_bind(receiver, "inproc://step2");
    assert(rc != -1);

    // 2.创建线程, 线程中创建PAIR1套接字, PAIR1套接字会向PAIR2发送消息
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, step1, arg);
    
    // 3.等待接收PAIR1发来消息
    char *string = s_recv(receiver);
    assert(string != NULL);
    free(string);
    // 关闭PAIR2套接字
    zmq_close(receiver);


    // 接收到PAIR1发来消息之后, 再开始向PAIR3发送通知

    
    // 4.重新创建一个PAIR2套接字, 该套接连接PAIR3
    void *xmitter = zmq_socket(arg, ZMQ_PAIR);
    assert(xmitter != NULL);
    rc = zmq_connect(xmitter, "inproc://step3");
    assert(rc != -1);

    // 5.向PAIR3发送通知
    printf("Step 2 ready, signaling step 3\n");
    rc = s_send(xmitter, "READY");
    assert(rc != -1);
    zmq_close(xmitter);

    return NULL;
}

static void *step1(void *arg)
{
    // 参数为上下文对象指针
    int rc;
    
    // 创建一个PAIR1套接字, 然后连接PAIR2套接字
    void *xmitter = zmq_socket(arg, ZMQ_PAIR);
    assert(xmitter != NULL);
    rc = zmq_connect(xmitter, "inproc://step2");
    assert(rc != -1);

    // 5.向PAIR2发送通知
    printf("Step 1 ready, signaling step 2\n");
    rc = s_send(xmitter, "READY");
    assert(rc != -1);
    zmq_close(xmitter);

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
