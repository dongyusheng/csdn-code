// rrclient.c
// https://blog.csdn.net/qq_41453285/article/details/106878960
// https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/rrclient.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

// 向socket发送数据, 数据为string
static int s_send(void *socket, char *string);
// 从socket接收数据, 并将数据以字符串的形式返回
static char *s_recv(void *socket);

int main()
{
    int rc;
    // 1.初始化上下文
    void *context = zmq_ctx_new();

    // 2.创建套接字、连接代理的ROUTER端
    void *requester = zmq_socket(context, ZMQ_REQ);
    rc = zmq_connect(requester, "tcp://localhost:5559");
    if(rc == -1)
    {
        perror("zmq_connect");
        zmq_close(requester);
        zmq_ctx_destroy(context);
        return -1;
    }

    // 3.循环发送、接收数据(10次)
    int request_nbr;
    for(request_nbr = 0; request_nbr < 10; request_nbr++)
    {
        // 4.先发送数据
        rc = s_send(requester, "Hello");
        if(rc < 0)
        {
            perror("s_send");
            zmq_close(requester);
            zmq_ctx_destroy(context);
            return -1;
        }
        
        // 5.等待响应
        char *reply = s_recv(requester);
        if(reply == NULL)
        {
            perror("s_recv");
            free(reply);
            zmq_close(requester);
            zmq_ctx_destroy(context);
            return -1;
        }
        printf("Reply[%d]: %s\n", request_nbr + 1, reply);
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
