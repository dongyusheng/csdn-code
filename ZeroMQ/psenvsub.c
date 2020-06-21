// psenvsub.c
// https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/psenvsub.c
// https://blog.csdn.net/qq_41453285/article/details/106882275
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zmq.h>
#include <assert.h>

// 向socket套接字发送string信息
static char *s_recv(void *socket);

int main()
{
    int rc;
    // 1.初始化上下文
    void *context = zmq_ctx_new();

    // 2.创建、绑定套接字
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    assert(subscriber != NULL);
    rc = zmq_connect(subscriber, "tcp://localhost:5563");
    assert(rc != -1);

    // 3.设置套接字选项, 设置自己订阅带有"B"的相关订阅信息
    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "B", 1);
    assert(rc != -1);

    
    // 4.循环接收消息
    while(1)
    {
        // 5.读取包头信息
        char *address = s_recv(subscriber);
        assert(address != NULL);

        // 判断是否是多部分消息, 这一步可以省略, 因为我们通过源码可以看到订阅者就是发送的多部分消息
        //int more;
        //size_t more_size = sizeof(more);
        //zmq_getsockopt(subscriber, ZMQ_RCVMORE, &more, &more_size);

        // 6.读取包体信息
        char *contents = s_recv(subscriber);
        assert(contents != NULL);
        printf("%s: %s\n", address, contents);

        free(address);
        free(contents);
    }

    // 7.关闭套接字、清除上下文
    zmq_close(subscriber);
    zmq_ctx_destroy(context);

    return 0;
}


static char *s_recv(void *socket)
{
    int rc;
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    
    rc = zmq_msg_recv(&msg, socket, 0);
    if(rc == -1)
        return NULL;

    char *string = (char*)malloc(rc + 1);
    memcpy(string, zmq_msg_data(&msg), rc);
    
    zmq_msg_close(&msg);

    string[rc] = 0;
    
    return string;
}
