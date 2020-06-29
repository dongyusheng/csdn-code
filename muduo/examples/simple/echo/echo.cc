// echo.cc
// https://blog.csdn.net/qq_41453285/article/details/107015436
#include "examples/simple/echo/echo.h"
#include "muduo/base/Logging.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

// using namespace muduo;
// using namespace muduo::net;

// 构造TcpServer对象，为TcpServer对象注册回调函数
EchoServer::EchoServer(muduo::net::EventLoop* loop,
                        const muduo::net::InetAddress& listenAddr)
    :server_(loop, listenAddr, "EchoServer")
{
    server_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, _1));
    server_.setMessageCallback(std::bind(&EchoServer::onMessage, this, _1, _2, _3));
}

// 调用TcpServer对象的start()函数，启动服务
void EchoServer::start()
{
    server_.start();
}

// 接收客户端连接，并打印相关信息
void EchoServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    // perrAddress(): 返回对方地址(以InetAddress对象表示IP和port)
    // localAddress(): 返回本地地址(以InetAddress对象表示IP和port)
    // connected()：返回bool值, 表明目前连接是建立还是断开

    LOG_INFO << "EchoServer - " << conn->perrAddress().toIpPort() << "->" 
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
}

// 接收客户端数据，并将数据原封不动的返回给客户端
// conn参数: 收到数据的那个TCP连接
// buf参数: 是已经收到的数据，buf的数据会累积，直到用户从中取走（retrieve） 数据。注意buf是指针，表明用户代码可以修改（消费）buffer
// time参数: 是收到数据的确切时间，即epoll_wait()返回的时间，注意这个时间通常比read()发生的时间略早，可以用于正确测量程序的消息处理延迟。另外，Timestamp对象采用pass-by-value，而不是pass-by-(const)reference， 这是有意的，因为在x86-64上可以直接通过寄存器传参
void EchoServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
                           muduo::net::Buffer* buf,
                           muduo::Timestamp time)
{
    // 将接收到的数据封装为一个消息
    muduo::string msg(buf->retrieveAllAsString());

    LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
             << "data received at " << time.toString();

    // 将消息再回送回去, 不必担心send(msg)是否完整地发送了数据，muduo网络库会帮我们管理发送缓冲区
    conn->send(msg);
}
