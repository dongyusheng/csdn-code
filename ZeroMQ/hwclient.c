/ /https://blog.csdn.net/qq_41453285/article/details/106878960
// https://github.com/dongyusheng/csdn-code/blob/master/ZeroMQ/hwclient.c
// hwclient.c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <zmq.h>

// 向socket发送数据, 数据为string
static int s_send(void *socket, char *string);
// 从socket接收数据, 并将数据以字符串的形式返回
static char *s_recv(void *socket);

int main()
{
    // 1.创建上下文
    void *context = zmq_ctx_new();

    // 2.创建、绑定套接字
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");

    int rc;
    // 3.循环发送数据、接收数据
    while(1)
    {
        // 4.发送数据
        char *request = "Hello";
        rc = s_send(requester, request);
        assert(rc > 0);

        // 5.接收回复数据
        char *reply = s_recv(requester);
        assert(reply != NULL);
        printf("Reply: %s\n", reply);
        free(reply);
    }

    // 6.关闭套接字、销毁上下文
    zmq_close(requester);
    zmq_ctx_destroy(context);

    return 0;
}

static int s_send(void *socket, char *string)
{
    int rc;
    
    zmq_msg_t msg;
    zmq_msg_init_size(&msg, 5);
    memcpy(zmq_msg_data(&msg), string, strlen(string));
    
    rc = zmq_msg_send(&msg, socket, 0);
    zmq_msg_close(&msg);

    return rc;
}

static char *s_recv(void *socket)
{
    int rc;
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    
    rc = zmq_msg_recv(&msg, socket, 0);
    if(rc == -1)
        return NULL;
    
    char *string = (char*)malloc(rc + 1);
    memcpy(string, zmq_msg_data(&msg), rc);
    zmq_msg_close(&msg);
    
    string[rc] = 0;
    return string;
}
