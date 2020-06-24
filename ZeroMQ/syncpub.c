// syncpub.c
// https://blog.csdn.net/qq_41453285/article/details/106949903
// 源码链接: 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <zmq.h>

// 我们等待10个订阅者
#define SUBSCRIBERS_EXPECTED 10

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

    // 2.创建PUB套接字, 用来发布数据
    void *publisher = zmq_socket(context, ZMQ_PUB);
    assert(publisher != NULL);
    rc = zmq_bind(publisher, "tcp://*:5561");
    assert(rc != -1);

    // 3.创建REP套接字, 用来接收订阅者发来订阅请求
    void *syncservice = zmq_socket(context, ZMQ_REP);
    assert(syncservice != NULL);
    rc = zmq_bind(syncservice, "tcp://*:5562");
    assert(rc != -1);

    // 4.等待10个订阅者发来订阅请求才开始发布数据
    printf("Waiting for subscribers\n");
    int subscribers = 0;
    while(subscribers < SUBSCRIBERS_EXPECTED)
    {
        // 从REP套接字上接收订阅者发来的订阅请求
        char *string = s_recv(syncservice);
        free(string);

        // 收到订阅请求, 回送应答给订阅者
        rc = s_send(syncservice, "");
        assert(rc != -1);
        subscribers++;
    }

    // 5.下面才开始真正的发布数据
    //    下面广播数据, 然后以一个END字符串表示发布数据结束
    printf("Broadcasting messages\n");
    int update_nbr;
    for(update_nbr = 0; update_nbr < 10; update_nbr++)
        s_send(publisher, "Rhubarb");
    s_send(publisher, "END");

    // 6.关闭套接字、销毁上下文
    zmq_close(publisher);
    zmq_close(syncservice);
    zmq_ctx_destroy(context);

    return 0;
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
