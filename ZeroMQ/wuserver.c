//wuserver.c
//https://blog.csdn.net/qq_41453285/article/details/106005939
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <zmq.h>
#include <string.h>

#define randof(num)  (int)((float)(num)*random()/(RAND_MAX + 1.0))
static int s_send (void *socket, char *string);

int main()
{
    //创建新的上下文
    void* context = zmq_ctx_new();

    //创建PUB套接字
    void* publisher = zmq_socket(context, ZMQ_PUB);
    int rc = zmq_bind(publisher, "tcp://*:5556");
    assert(rc == 0);
    rc = zmq_bind(publisher, "ipc://weather.ipc");
    assert(rc == 0);

    //初始化随机数发生器
    srandom((unsigned)time(NULL));

    //循环发送数据
    while(1)
    {
        //随机生成邮政编码、温度、适度
        int zipcode, temperature, relhumidity;
        zipcode     = randof(100000);
        temperature = randof(215) - 80;
        relhumidity = randof(50) + 10;

        //将消息发送给所有的订阅者
        char update[20];
        sprintf(update, "%05d %d %d", zipcode, temperature, relhumidity);
        s_send(publisher, update);
    }

    zmq_close(publisher);
    zmq_ctx_term(context);
    return 0;
}

//向指定的socket发送字符串
static int s_send (void *socket, char *string) 
{
    int rc;
    zmq_msg_t message;
    zmq_msg_init (&message);
    
    memcpy (zmq_msg_data (&message), string, strlen (string));
    rc = zmq_msg_send (&message, socket, 0);
    assert (!rc);
    
    zmq_msg_close (&message);
    return (rc);
}
