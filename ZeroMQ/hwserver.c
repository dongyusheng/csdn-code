//hwserver.c
//https://blog.csdn.net/qq_41453285/article/details/106005183
//https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/hwserver.c
#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main()
{
    //创建新的上下文
    void* context = zmq_ctx_new();

    //创建套接字、绑定地址
    void* responder = zmq_socket(context, ZMQ_REP);
    zmq_bind(responder, "tcp://*:5555");

    //循环等待接收数据、回送数据
    while(1)
    {
        //等待接收客户端的数据
        zmq_msg_t request;
        zmq_msg_init(&request);
        zmq_msg_recv(&request, responder, 0);
        printf("Received Hello\n");
        zmq_msg_close(&request);

        //这个中间可以做一些事情，我们在此处休眠1秒
        sleep(1);

        //返回数据给客户端
        zmq_msg_t reply;
        zmq_msg_init_size(&reply, 5);
        memcpy(&reply, "World", 5);
        zmq_msg_send(&reply, responder, 0);
        zmq_msg_close(&reply);
    }

    //关闭套接字、清除上下文
    zmq_close(responder);
    zmq_ctx_term(&context);

    return 0;
}
