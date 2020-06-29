// main.cc
// https://blog.csdn.net/qq_41453285/article/details/107015436
#include "examples/simple/echo/echo.h"

#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"

#include <unistd.h>

// using namespace muduo;
// using namespace muduo::net;

int main()
{
    // 1.打印进程ID
    LOG_INFO << "pid = " << getpid();

    // 2.初始化EventLoop、InetAddress对象,
    muduo::net::EventLoop loop;
    muduo::net::InetAddress listenAddr(2007);

    // 3.创建EchoServer, 启动服务
    EchoServer server(&loop, listenAddr);
    server.start();

    // 4.事件循环
    loop.loop();
}
