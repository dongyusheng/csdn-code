//taskwork.c
//https://blog.csdn.net/qq_41453285/article/details/106922554
//https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/taskwork.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <zmq.h>

// 从socket上接收数据并返回
static char *s_recv(void *socket);

// 向套接字socket发送消息string
static int s_send(void *socket, char *string);


// 休眠msecs毫秒
static int s_sleep(int msecs);

int main()
{
    int rc;
    
    // 1.初始化新的上下文
    void *context = zmq_ctx_new();
    assert(context != NULL);

    // 2.创建套接字、连接发生器
    //   该套接字用来从发生器接收数据
    void *reciver = zmq_socket(context, ZMQ_PULL);
    assert(reciver != NULL);
    rc = zmq_connect(reciver, "tcp://localhost:5557");
    assert(rc != -1);
    
    // 3.创建套接字、连接接收器
    //   该套接字用来向接收器发送数据
    void *sender = zmq_socket(context, ZMQ_PUSH);
    assert(sender != NULL);
    rc = zmq_connect(sender, "tcp://localhost:5558");
    assert(rc != -1);

    // 4.永久循环处理任务
    while(1)
    {
        // 5.从发生器接收数据, 这里接收到的将是一个毫秒数
        char *string = s_recv(reciver);
        assert(string != NULL);
        //打印接收到的毫秒数
        fflush(stdout);
        printf("%s.", string);

        // 然后工人休眠指定的毫秒继续工作
        s_sleep(atoi(string));
        free(string);

        
        // 6.工人在处理完任务之后, 发送一条消息给接收器, 表示完成了一条任务
        rc = s_send(sender, "");
    }

    // 7.关闭套接字、销毁上下文
    zmq_close(reciver);
    zmq_close(sender);
    zmq_ctx_destroy(context);

    return 0;
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
    if(string == NULL)
        return NULL;
    memcpy(string, zmq_msg_data(&msg), rc);

    string[rc] = 0;
    
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

static int s_sleep(int msecs)
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
