//wuserver.c
//https://blog.csdn.net/qq_41453285/article/details/106877202
//https://github.com/dongyusheng/csdn-code/blob/master/ZeroMQ/wuserver.c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <zmq.h>

// 随机生成0...num-1的随机数
#define randof(num)  (int) ((float) (num) * random () / (RAND_MAX + 1.0))

// 将string消息格式化为zmq_meg_t对象, 然后发往socket套接字上
static int s_send(void *socket, char *string);

int main()
{
    // 1.初始化上下文
    void *context = zmq_ctx_new();

    // 2.创建、绑定套接字
    void *publisher = zmq_socket(context, ZMQ_PUB);
    assert(publisher != NULL);

    // 此处我们将发布者绑定到一个tcp节点上和一个ipc节点上, 但是本案例我们只使用tcp, ipc那个只是演示说明zmq的套接字可以绑定到多个节点上
    int rc = zmq_bind(publisher, "tcp://*:5555");
    assert(rc == 0);
    rc = zmq_bind(publisher, "ipc://weather.ipc");
    assert(rc == 0);

    // 3.初始化随机数发生器
    srandom((unsigned)time(NULL));

    // 4.循环发送数据
    while(1)
    {
        // 5.随机生成邮政编码、温度、适度
        int zipcode, temperature, relhumidity;
        zipcode = randof(100000);
        temperature = randof(215) - 80;
        relhumidity = randof(50) + 10;

        // 6.将消息发送给所有的订阅者
        char update[20];
        sprintf(update, "%05d %d %d", zipcode, temperature, relhumidity);
        rc = s_send(publisher, update);
        assert(rc);
    }

    // 7.关闭套接字、销毁上下文
    zmq_close(publisher);
    zmq_ctx_destroy(context);

    return 0;
}

static int s_send(void *socket, char *string)
{
    // 初始化一个zmq_msg_t对象, 分配的大小为string的大小
    zmq_msg_t msg;
    zmq_msg_init_size(&msg, strlen(string));
    memcpy(zmq_msg_data(&msg), string, strlen(string));

    // 发送数据
    int rc = zmq_msg_send(&msg, socket, 0);

    // 关闭zmq_msg_t对象
    zmq_msg_close(&msg);

    return rc;
}
