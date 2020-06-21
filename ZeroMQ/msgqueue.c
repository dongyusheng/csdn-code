// msgqueue.c
// https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/msgqueue.c
// https://blog.csdn.net/qq_41453285/article/details/106887035
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
 
int main()
{
    int rc;
    // 1.初始化上下文
    void *context = zmq_ctx_new();
 
    // 2.创建、绑定套接字
    void *frontend = zmq_socket(context, ZMQ_ROUTER);
    void *backend = zmq_socket(context, ZMQ_DEALER);
    // ZMQ_ROUTER绑定到5559, 接收客户端的请求
    rc = zmq_bind(frontend, "tcp://*:5559");
    if(rc == -1)
    {
        perror("zmq_bind");
        zmq_close(frontend);
        zmq_close(backend);
        zmq_ctx_destroy(context);
        return -1;
    }
    // ZMQ_DEALER绑定到5560, 接收服务端的回复
    rc = zmq_bind(backend, "tcp://*:5560");
    if(rc == -1)
    {
        perror("zmq_bind");
        zmq_close(frontend);
        zmq_close(backend);
        zmq_ctx_destroy(context);
        return -1;
    }

    // 3.启动代理
    zmq_proxy(frontend, backend, NULL);
 
    // 4.关闭套接字、销毁上下文
    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(context);
    
    return 0;
}
