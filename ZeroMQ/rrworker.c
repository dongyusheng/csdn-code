// rrworker.c
// https://blog.csdn.net/qq_41453285/article/details/106878960
// https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/rrworker.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

    // 2.创建套接字、连接代理的DEALER端
    void *responder = zmq_socket(context, ZMQ_REP);
    rc = zmq_connect(responder, "tcp://localhost:5560");
    if(rc == -1)
    {
        perror("zmq_connect");
        zmq_close(responder);
        zmq_ctx_destroy(context);
        return -1;
    }

    // 3.循环接收、响应
    while(1)
    {
        // 4.先等待接收数据
        char *request = s_recv(responder);
        if(request == NULL)
        {
            perror("s_recv");
            free(request);
            zmq_close(responder);
            zmq_ctx_destroy(context);
            return -1;
        }
        printf("Request: %s\n", request);
        free(request);

        // 休眠1秒再进行响应
        sleep(1);
        
        // 5.响应
        rc = s_send(responder, "World");
        if(rc < 0)
        {
            perror("s_send");
            zmq_close(responder);
            zmq_ctx_destroy(context);
            return -1;
        }
    }

    // 6.关闭套接字、销毁上下文
    zmq_close(responder);
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
