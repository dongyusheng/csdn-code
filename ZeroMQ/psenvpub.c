// psenvpub.c
// https://github.com/dongyusheng/csdn-code/blob/master/ZeroMQ/psenvpub.c
// https://blog.csdn.net/qq_41453285/article/details/106882275
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zmq.h>
#include <unistd.h>
#include <assert.h>

// 向socket套接字发送string信息
static int s_send(void *socket, char *string);
// 向socket发送消息, 发送时zmq_msg_send()函数使用ZMQ_SNDMORE套接字选项
static int s_sendmore(void *socket, char *string);


int main()
{
    int rc;
    // 1.初始化上下文
    void *context = zmq_ctx_new();

    // 2.创建、绑定套接字
    void *publisher = zmq_socket(context, ZMQ_PUB);
    assert(publisher != NULL);
    
    rc = zmq_bind(publisher, "tcp://*:5563");
    assert(rc != -1);

    // 3.循环发送消息
    while(1)
    {
        // 4.发送A类型的消息
        rc = s_sendmore(publisher, "A");                      //包头
        assert(rc != -1);
        rc = s_send(publisher, "We dont't want to see this"); //包体
        assert(rc != -1);

        // 5.发送B类型的消息
        rc = s_sendmore(publisher, "B");                      //包头
        assert(rc != -1);
        rc = s_send(publisher, "We would like to see this"); //包体
        assert(rc != -1);

        sleep(1);
    }

    // 6.关闭套接字、清除上下文
    zmq_close(publisher);
    zmq_ctx_destroy(context);

    return 0;
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

static int s_sendmore(void *socket, char *string)
{
    int rc;
    
    zmq_msg_t msg;
    zmq_msg_init_size(&msg, strlen(string));
    memcpy(zmq_msg_data(&msg), string, strlen(string));
    
    rc = zmq_msg_send(&msg, socket, ZMQ_SNDMORE);
    
    zmq_msg_close(&msg);
    
    return rc;
}
