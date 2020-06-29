// finger04.cc
// https://blog.csdn.net/qq_41453285/article/details/107015436
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"

using namespace muduo;
using namespace muduo::net;

void onMessage(const TcpConnectionPtr& conn,
               Buffer* buf,
               Timestamp receiveTime)
{
    if (buf->findCRLF())
    {
        conn->shutdown();
    }
}

int main()
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(1079), "Finger");
    server.setMessageCallback(onMessage);
    server.start();
    loop.loop();
}
