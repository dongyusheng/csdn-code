//tasksink.c
//https://blog.csdn.net/qq_41453285/article/details/106922554
//https://github.com/dongyusheng/csdn-code/blob/master/ZeroMQ/tasksink.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <zmq.h>

// 从socket中接收数据
static char* s_recv(void* socket);

// 返回当前系统时钟,以毫秒返回
static int64_t s_clock (void);

int main()
{
    // 1.创建新的上下文
    void *context = zmq_ctx_new();

    // 2.创建、绑定套接字
    //   发生器、工人都会连接这个套接字, 并向这个套接字发送数据
    void *receiver = zmq_socket(context, ZMQ_PULL);
    zmq_bind(receiver, "tcp://*:5558");

    // 3.这是接收的第一条消息, 从发生器那里接收的, 是发生器用来告诉当前接收器, 表示开始工作了
    char *string = s_recv(receiver);
    free(string);

    // 4.启动时钟
    int64_t start_time = s_clock();

    // 5.从工人那里接收100个确认, 因为发生器只分配了100个任务给工人
    int task_nbr;
    for(task_nbr = 0; task_nbr < 100; task_nbr++)
    {
        // 6.接收数据
        char *string = s_recv(receiver);
        free(string);

        // 7.每接收10个任务打印一次:号, 其余打印.
        if((task_nbr / 10) * 10 == task_nbr)
            printf(":");
        else
            printf(".");
        fflush(stdout);
    }
    
    // 8.处理完成之后打印一下总共执行了多长时间, 也就是工人一共工作了多长时间
    printf("Total elapsed time: %d msec\n",(int)(s_clock() - start_time));

    // 9.关闭套接字、销毁上下文
    zmq_close(receiver);
    zmq_ctx_destroy(context);
    
    return 0;
}

static char* s_recv(void* socket)
{
    zmq_msg_t msg;
    zmq_msg_init(&msg);
 
    int rc = zmq_msg_recv(&msg, socket, 0);
    if(rc == -1)
        return NULL;
 
    char *string = (char*)malloc(rc + 1);
    memcpy(string, zmq_msg_data(&msg), rc);
 
    zmq_msg_close(&msg);
    string[rc] = 0;
    
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
