//taskvent.c
//https://blog.csdn.net/qq_41453285/article/details/106922554
//https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/taskvent.c
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <zmq.h>

#define randof(num)  (int)((float)(num)*random()/(RAND_MAX + 1.0))
static int s_send (void *socket, char *string);

int main()
{
    //创建一个新的上下文
    void *context = zmq_ctx_new();

    //用于发送消息的套接字
    void *sender = zmq_socket(context, ZMQ_PUSH);
    zmq_bind(sender, "tcp://*:5557");

    //用于发送批次开始消息的套接字
    void *sink = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(sink, "tcp://localhost:5558");

    printf("Press Enter when the workers are ready:");
    getchar();
    printf("Sending tasks to workers...\n");

    //第一个消息发送一个"0"，它表示批次的开始
    s_send(sink, 0);

    //初始化随机数发生器
    srandom((unsigned)time(NULL));

    //发送100个任务
    int task_nbr;
    int total_msec = 0;
    int workload;
    for(task_nbr = 0; task_nbr < 100; task_nbr++)
    {
        workload = randof(100) + 1; //生成一个毫秒数，1-100之间
        total_msec += workload;
        char string[10];
        sprintf(string, "%d", workload);
        s_send(sender, string);     //将毫秒数发送给工人
    }
    printf("Total expected cost: %d msec\n", total_msec);
    //休眠一会,给zeromq时间来传递
    sleep(1);

    //善后处理
    zmq_close(sink);
    zmq_close(sender);
    zmq_ctx_term(context);
    return 0;
}

//向指定的套接字发送字符串
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
