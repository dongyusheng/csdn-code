// wuproxy.c
// https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/wuproxy.c
// https://blog.csdn.net/qq_41453285/article/details/106888222
#include <stdio.h>
#include <zmq.h>

int main()
{
    // 1.创建新的上下文
    void *context = zmq_ctx_new();

    // 2.前端套接字, 用于连接内部的天气服务器
    void *frontend = zmq_socket(context, ZMQ_XSUB);
    zmq_connect(frontend, "tcp://localhost:5556");

    // 3.后端套接字, 用来处理外部的订阅者的请求
    void *backend = zmq_socket(context, ZMQ_XPUB);
    zmq_bind(backend, "tcp://111.229.177.161:8100");

    // 4.持续运行代理
    zmq_proxy(frontend, backend, NULL);

    // 5.关闭套接字、清除上下文
    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(context);

    return 0;
}
