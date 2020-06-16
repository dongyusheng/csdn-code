//taskwork.c
//https://blog.csdn.net/qq_41453285/article/details/106009184
//https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/taskwork.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <zmq.h>

//从套接字接收ZeroMQ字符串，并将其转换为C/C++字符串
static char* s_recv(void* sokcet);
//向指定的套接字发送消息
static int s_send (void *socket, char *string);
//休眠指定的毫秒数
static void s_sleep (int msecs);

int main()
{
    //创建新的上下文
    void *context = zmq_ctx_new();

    //用于接收消息的套接字
    void *reciver = zmq_socket(context, ZMQ_PULL);
    zmq_connect(reciver, "tcp://localhost:5557");

    //用于发送消息的套接字
    void *sender = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(sender, "tcp://localhost:5558");

    //永远地处理任务
    while(1)
    {
        //接收消息
        char *string = s_recv(reciver);

        //用于查看器的简易过程指示器
        fflush(stdout);
        printf("%s.", string);

        //休眠
        s_sleep(atoi(string));
        free(string);

        //发送空消息
        s_send(sender,"");
    }

    //善后处理
    zmq_close(reciver);
    zmq_close(sender);
    zmq_ctx_term(context);
    return 0;
}

static char* s_recv(void* socket)
{
    zmq_msg_t message;
    zmq_msg_init(&message);
 
    int size = zmq_msg_recv(&message, socket, 0);
    if(size == -1)
        return NULL;
 
    char *string = (char*)malloc(size + 1);
    memcpy(string, zmq_msg_data(&message), size);
 
    zmq_msg_close(&message);
    string[size] = 0;
    return string;
}

static int s_send (void *socket, char *string) 
{
    int rc;
    zmq_msg_t message;
    zmq_msg_init (&message);
    memcpy (zmq_msg_data (&message), string, strlen (string));
    rc = zmq_msg_send (&message, socket, 0);
    assert (!rc);
    zmq_msg_close (&message);
    return (rc);
}

static void s_sleep (int msecs)
{
#if (defined (WIN32))
    Sleep (msecs);
#else
    struct timespec t;
    t.tv_sec  =  msecs / 1000;
    t.tv_nsec = (msecs % 1000) * 1000000;
    nanosleep (&t, NULL);
#endif
}
