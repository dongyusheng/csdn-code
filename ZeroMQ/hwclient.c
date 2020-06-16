//hwclient.c
//https://blog.csdn.net/qq_41453285/article/details/106005183
//https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/hwclient.c
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    ////创建新的上下文
    void* context = zmq_ctx_new();
    printf("Connecting to hello world server...\n");

    //创建套接字、连接服务器
    void* requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");

    int request_nbr;
    for(request_nbr = 0; request_nbr < 10; request_nbr++)
    {
        //发送数据给服务端
        zmq_msg_t request;
        zmq_msg_init_size(&request, 5);
        memcpy(zmq_msg_data(&request), "Hello", 5);
        printf("Sending hello %d...\n", (request_nbr + 1));
        zmq_msg_send(&request, requester, 0);
        zmq_msg_close(&request);

        //等待接收服务端回送数据
        zmq_msg_t reply;
        zmq_msg_init(&reply);
        zmq_msg_recv(&reply, requester, 0);
        printf("Received World %d\n", (request_nbr + 1));
        zmq_msg_close(&reply);
    }

    //善后处理
    sleep(2);
    zmq_close(requester);
    zmq_ctx_term(context);
    return 0;
}
