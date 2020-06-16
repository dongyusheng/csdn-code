//hwserver.cpp
#include <zmq.hpp>
#include <iostream>
#include <unistd.h>
#include <string>

int main()
{
    //创建新的上下文
    zmq::context_t context(1);

    //创建套接字、绑定地址
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

    //循环发送数据、接收数据
    while(true)
    {
        //阻塞接收数据
        zmq::message_t request;
        socket.recv(&request);
        std::cout << "Received Hello" << std::endl;

        //这个中间可以做一些事情，我们在此处休眠1秒
        sleep(1);

        //发送数据
        zmq::message_t reply(5);
        memcpy((void*)reply.data(), "World", 5);
        socket.send(reply);
    }
}
