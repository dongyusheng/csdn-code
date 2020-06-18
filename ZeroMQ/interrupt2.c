// interrupt2.c
#include <stdio.h>
#include <zmq.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


#define S_NOTIFY_MSG " "  // 消息, 在管道中传输, 用来表示有信号来到
#define S_ERROR_MSG "Error while writing to self-pipe.\n"

static int s_fd;

// 信号处理函数
static void s_signal_handler(int signal_value)
{
    //向管道的写端写入数据, 表示接收到信号了, 那么在zmq_poll()中轮询的管道的读端就会收到退出信息, 进而清除程序退出
    int rc = write(s_fd, S_NOTIFY_MSG, 1);
    // 如果此处写失败了, 那么管道的读端就不会知道有信号来了, 从而导致信号处理丢失, 因此此处我们需要自己退出程序, 但是丢失了对资源的关闭清除
    if(rc == -1)
    {
        write(STDOUT_FILENO, S_ERROR_MSG, sizeof(S_ERROR_MSG) - 1);
        exit(EXIT_FAILURE);
    }
}

// 初始化信号处理
void s_catch_signal(int fd)
{
    // 将管道的写端赋值给s_fd
    s_fd = fd;
    
    // 信号处理结构
    struct sigaction action;
    //信号处理函数
    action.sa_handler = s_signal_handler;
    // 标志,不需要设置为SA_RESTART(由此信号中断的系统调用自动重启动), 因为我们已经对各种函数的返回值进行了EINTR的判断, 如果函数返回EINTR我们一律不报错继续运行
    action.sa_flags = 0;                  
    sigemptyset(&action.sa_mask);

    // 添加对SIGINT和SIGTERM信号的处理
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

int main()
{
    // 初始化上下文
    void *context = zmq_ctx_new();

    // 初始化绑定套接字
    void *socket = zmq_socket(context, ZMQ_REP);
    zmq_bind(socket, "tcp://*:5555");


    // 创建管道,
    // pipefds[0]是读端, pipefds[1]是写端. 向pipefds[1]中写入数据, 在pipefds[0]中会读取到
    int pipefds[2];
    int rc;
    rc = pipe(pipefds);
    if(rc == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // 将管道的写端送入信号处理函数中, 如果有信号来就会向管道的写端写入数据
    s_catch_signal(pipefds[1]);
    
    // 将管道的读端设置为非阻塞的
    int flags = fcntl(pipefds[0], F_GETFL, 0);
    if(flags == -1)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    rc = fcntl(pipefds[0], F_SETFL, flags | O_NONBLOCK);
    if(rc == -1)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    // 将管道的读端和套接字加入zmq_poll中
    zmq_pollitem_t items[2] = {
        { NULL, pipefds[0], ZMQ_POLLIN, 0},
        { socket, 0, ZMQ_POLLIN, 0}
    };

    // 轮询数据
    while(1)
    {
        // zmq_poll为阻塞模式
        rc = zmq_poll(items, 2, -1);
        if(rc == -1)
        {
            if(errno == EINTR) { continue; } //如果是阻塞期间接收到了信号, 那么继续进行zmq_poll, 在下次一次zmq_poll的时候管道读端就会变为可读状态
            perror("zmq_poll");
            exit(EXIT_FAILURE);
        }
        else if(rc == 0)
        {
            continue;
        }
        else
        {
            // 如果管道的读端可读, 那么说明有信号来了, 因为在信号处理函数中会向管道的写端写入数据
            if(items[0].revents & ZMQ_POLLIN)
            {
                // 读取之后退出循环, 做清除工作
                char buff[1];
                read(items[0].fd, buff, 1); //此处读取一次, 将管道里的消息读取出来清除
                printf("Interrupt received, killing server...\n");
                break;
            }

            // 如果套接字可读
            if(items[1].revents & ZMQ_POLLIN)
            {
                char buff[255];
                // 使用非阻塞读取, 这样我们就不会因为阻塞而错过管道里的消息了
                rc = zmq_recv(items[1].socket, buff, sizeof(buff), ZMQ_DONTWAIT);
                if(rc = -1)
                {
                    // 如果zmq_recv()读取失败, 那么判断一下返回值
                    if(errno == EAGAIN) { continue; } // EAGIN表示zmq_recv()是在非阻塞模式下返回的且没有消息可读, 那么继续进行zmq_poll
                    if(errno == EINTR) { continue; } // 如果是信号打断的, 那么继续进行zmq_poll, 信号处理函数会对信号处理, 此处无须关心
                    perror("zmq_recv");
                    exit(EXIT_FAILURE);
                }

                printf("Recv: %s\n", buff);

                // 之后发送数据等等, 此处忽略
            }
        }
    }

    // 关闭套接字、销毁上下文
    printf("Cleaning up...\n");
    zmq_close(socket);
    zmq_ctx_destroy(context);
    
    return 0;
}

