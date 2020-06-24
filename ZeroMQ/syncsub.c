// syncsub.c
// https://blog.csdn.net/qq_41453285/article/details/106949903
// https://github.com/dongyusheng/csdn-code/blob/master/ZeroMQ/syncsub.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <zmq.h>

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

    // 2.创建SUB套接字, 连接发布者
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    assert(subscriber != NULL);
    rc = zmq_connect(subscriber, "tcp://localhost:5561");
    assert(rc != -1);

    // 3.设置订阅过滤
    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);
    assert(rc != -1);
    
    // 休眠1秒
    // 我们不能保证SUB在下面REQ/REP对话完成的时候就能建立连接, 因此此处休眠一秒, 保证SUB一定能与PUB建立连接
    sleep(1);

    // 4.创建REQ套接字, 用来给发布者发送订阅请求, 只需要发送一次即可
    void *syncclient = zmq_socket(context, ZMQ_REQ);
    assert(syncclient != NULL);
    rc = zmq_connect(syncclient, "tcp://localhost:5562");
    assert(rc != -1);

    // 5.给发布者发送订阅请求, 并接收发布者给自己返回确认
    rc = s_send(syncclient, "");
    assert(rc != -1);
    char *string = s_recv(syncclient);
    assert(string != NULL);
    free(string);

    // 6.下面才开始真正的接收发布者的发布数据
    int update_nbr = 0;
    while(1)
    {
        // 接收数据
        char *string = s_recv(subscriber);
        // 如果接收到END说明发布数据全部接收完了, 退出循环
        if(strcmp(string , "END") == 0)
        {
            free(string);
            break;
        }
        free(string);
        update_nbr++;
    }
    printf("Received %d updates\n", update_nbr);

    // 7.关闭套接字、销毁上下文
    zmq_close(subscriber);
    zmq_close(syncclient);
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
