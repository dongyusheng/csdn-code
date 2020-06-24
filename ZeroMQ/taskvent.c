//taskvent.c
//https://blog.csdn.net/qq_41453285/article/details/106922554
//https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/taskvent.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <zmq.h>

#define randof(num)  (int)((float)(num) * random() / (RAND_MAX + 1.0))

// 向套接字socket发送消息string
static int s_send(void *socket, char *string);

int main()
{
    int rc;
    
    // 1.创建新的上下文
    void *context = zmq_ctx_new();
    assert(context != NULL);

    // 2.创建PUSH套接字、绑定套接字,
    //   工人会连接这个套接字, 用来给工人发送消息的
    void *sender = zmq_socket(context, ZMQ_PUSH);
    assert(sender != NULL);
    rc = zmq_bind(sender, "tcp://*:5557");
    assert(rc != -1);

    // 3.创建PUSH套接字、并连接到接收器,
    //   该套接字给接收器发送一个消息, 告诉接收器开始工作, 只使用一次
    void *sink = zmq_socket(context, ZMQ_PUSH);
    assert(sink != NULL);
    rc = zmq_connect(sink, "tcp://localhost:5558");
    assert(rc != -1);

    // 4.输入回车, 回车之后发生器开始产生任务, 并且向接收器发送一条消息, 用于指示当前发生器要开始工作了
    printf("Press Enter when the workers are ready:");
    getchar();
    printf("Sending tasks to workers...\n");

    // 随意发什么, 此处我们发送字符0
    // 如果接收器未工作, 则s_send()阻塞
    rc = s_send(sink, "0");
    assert(rc != -1);

    // 5.初始化随机数发生器
    srandom((unsigned)time(NULL));

    // 6.生成100个任务, 然后将任务发送给工人
    int task_nbr;
    int total_msec = 0;
    int workload;
    for(task_nbr = 0; task_nbr < 100; ++task_nbr)
    {
        workload = randof(100) + 1;      //生成一个毫秒数, 1-100之间
        total_msec += workload;          //加到总的毫秒数上
        char string[10];
        sprintf(string, "%d", workload);
        s_send(sender, string);          //将毫秒发送给工人
    }
    //打印总的毫秒数
    printf("Total expected cost: %d msec\n", total_msec);
    
    // 7.休眠1秒, 给ZeroMQ时间来传递消息
    sleep(1);

    // 8.关闭套接字、销毁上下文
    zmq_close(sender);
    zmq_close(sink);
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
