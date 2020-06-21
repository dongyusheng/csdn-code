// rrbroker.c
// https://blog.csdn.net/qq_41453285/article/details/106878960
// https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/rrbroker.c
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
    // 3.初始化轮询集合
    zmq_pollitem_t items[] = {
        { frontend, 0, ZMQ_POLLIN, 0 },
        { backend, 0, ZMQ_POLLIN, 0 }
    };

    // 4.在套接字上切换消息
    while(1)
    {
        zmq_msg_t msg;
        //多部分消息检测
        int more;     

        // 5.调用zmq_poll轮询消息
        rc = zmq_poll(items, 2, -1);
        //zmq_poll出错
        if(rc == -1)     
        {
             perror("zmq_poll");
            zmq_close(frontend);
            zmq_close(backend);
            zmq_ctx_destroy(context);
            return -1;
        }
        //zmq_poll超时
        else if(rc == 0) 
            continue;
        else
        {
            // 6.如果ROUTER套接字有数据来
            if(items[0].revents & ZMQ_POLLIN)
            {
                while(1)
                {
                    // 从ROUTER上接收数据, 这么数据是客户端发送过来的"Hello"
                    zmq_msg_init(&msg);
                    zmq_msg_recv(&msg, frontend, 0);

                    // 查看是否是接收多部分消息, 如果后面还有数据要接收, 那么more会被置为1
                    size_t more_size = sizeof(more);
                    zmq_getsockopt(frontend, ZMQ_RCVMORE, &more, &more_size);

                    // 接收"Hello"之后, 将数据发送到DEALER上, DEALER会将"Hello"发送给服务端
                    zmq_msg_send(&msg, backend, more ? ZMQ_SNDMORE : 0);
                    zmq_msg_close(&msg);

                    // 如果没有多部分数据可以接收了, 那么退出循环
                    if(!more)
                        break;
                }
            }
            // 7.如果DEALER套接字有数据来
            if(items[1].revents & ZMQ_POLLIN)
            {
                
                while(1)
                {
                    // 接收服务端的响应"World"
                    zmq_msg_init(&msg);
                    zmq_msg_recv(&msg, backend, 0);

                    // 查看是否是接收多部分消息, 如果后面还有数据要接收, 那么more会被置为1
                    size_t more_size = sizeof(more);
                    zmq_getsockopt(backend, ZMQ_RCVMORE, &more, &more_size);

                    // 接收"World"之后, 将数据发送到ROUTER上, ROUTER会将"World"发送给客户端
                    zmq_msg_send(&msg, frontend, more ? ZMQ_SNDMORE : 0);
                    zmq_msg_close(&msg);

                    // 如果没有多部分数据可以接收了, 那么退出循环
                    if(!more)
                        break;
                }
            }
        }
    }

    // 关闭套接字、销毁上下文
    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(context);
    
    return 0;
}
