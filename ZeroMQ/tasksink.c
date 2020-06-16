//tasksink.c
//https://blog.csdn.net/qq_41453285/article/details/106009184
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <zmq.h>
#include <sys/time.h>

static char* s_recv(void* sokcet);
//返回当前系统时钟,以毫秒返回
static int64_t s_clock (void);

int main()
{
    //创建新的上下文
    void *context = zmq_ctx_new();

    //创建套接字,用于接收从
    void *receiver = zmq_socket(context, ZMQ_PULL);
    zmq_bind(receiver, "tcp://*:5558");

    //等待批次的开始
    char *string = s_recv(receiver);
    free(string);

    //启动时钟
    int64_t start_time = s_clock();

    //处理100个确认
    int task_nbr;
    for(task_nbr = 0; task_nbr < 100; task_nbr++)
    {
        char *string = s_recv(receiver);
        free(string);
        if((task_nbr / 10) * 10 == task_nbr)
            printf(":");
        else
            printf(".");
        fflush(stdout);
    }
    //打印一下总共执行了多长时间
    printf("Total elapsed time: %d msec\n",(int)(s_clock() - start_time));

    //善后处理
    zmq_close(receiver);
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

static int64_t s_clock (void)
{
#if (defined (WIN32))
    SYSTEMTIME st;
    GetSystemTime (&st);
    return (int64_t) st.wSecond * 1000 + st.wMilliseconds;
#else
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (int64_t) (tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
}
