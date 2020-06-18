#include <stdio.h>
#include <zmq.h>
#include <signal.h>

// 表示是否接收到信号, 默认为0表示没有接收到信号
static int s_interrupted = 0;
// 信号处理函数, 在内部将s_interrupted设置为1
static void s_signal_handler(int signal_value)
{
    s_interrupted = 1;
}

void s_catch_signal()
{
    // 信号处理结构体, 关于这些接口如何使用请参阅: https://blog.csdn.net/qq_41453285/article/details/89278415
    struct sigaction action;
    action.sa_handler = s_signal_handler; //信号处理函数
    action.sa_flags = 0;                  //对信号进行哪些特殊处理
    sigemptyset(&action.sa_mask);         //将信号屏蔽集清空,

    // 添加对SIGINT、SIGTERM信号的处理
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

int main()
{
    // 初始化上下文
    void *context = zmq_ctx_new();

    // 创建绑定套接字
    void *socket = zmq_socket(context, ZMQ_REP);
    zmq_bind(socket, "tcp://*:5555");

    // 添加信号处理函数
    s_catch_signal();

    // 循环对消息进行处理
    while(1)
    {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        // recv会阻塞读, 当接收到信号时会返回-1停止阻塞, 我们不会其返回值做判断, 直接对s_interrupted对判断
        zmq_msg_recv(&msg, socket, 0);

        // 如果接收到信号, 那么s_interrupted就会变为1, 那么就退出循环, 清除退出程序
        if(s_interrupted)
        {
            printf("interrupt received, killing server...\n");
            break;
        }
    }

    // 关闭消息、销毁上下文
    zmq_close(socket);
    zmq_ctx_destroy(context);

    return 0;
}

