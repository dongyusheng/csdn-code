// echo.h
// https://blog.csdn.net/qq_41453285/article/details/107015436
#ifndef MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H
#define MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H

#include <muduo/net/TcpServer.h>

class EchoServer
{
public:
    // 构造函数
    EchoServer(muduo::net::EventLoop* loop,
                const muduo::net::InetAddress& listenAddr);

    // 启动服务
    void start();
private:
    // 响应客户端连接
    void onConnection(const muduo::net::TcpConnectionPtr& conn);

    // 响应客户端消息
    void onMessage(const muduo::net::TcoConnectionPtr& conn,
                    muduo::net::Buffer* buf, 
                    muduo::Timestamp time);

    // TcpServer对象
    muduo::net::TcpServer server_;
};

#endif  // MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H
