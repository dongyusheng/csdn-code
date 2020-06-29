// finger03.cc
// https://blog.csdn.net/qq_41453285/article/details/107015436
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"

using namespace muduo;
using namespace muduo::net;

void onConnection(const TcpConnectionPtr& conn)
{
    if (conn->connected())
    {
        conn->shutdown();
    }
}

int main()
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(1079), "Finger");
    server.setConnectionCallback(onConnection);
    server.start();
    loop.loop();
}
