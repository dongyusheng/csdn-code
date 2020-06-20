// https://blog.csdn.net/qq_41453285/article/details/106877202
// https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/wuclient.c
// wuclient.c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <zmq.h>
 
// 从socket接收数据, 并将数据返回
char *s_recv(void *socket);
 
int main(int argc, char *argv[])
{
    // 1.初始化上下文
    void *context = zmq_ctx_new();
 
    // 2.创建套接字、连接发布者
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    assert(subscriber != NULL);
    int rc = zmq_connect(subscriber, "tcp://localhost:5555");
    assert(rc == 0);
 
    // 3.因为自己是订阅者, 因此需要使用设置过滤器, 显式指定自己是否需要接收什么类型的消息
    //   程序运行时可以输入参数, 参数代表邮政编码, 如果参数为空, 那么就过滤10001的消息
    char *filter = (argc > 1) ? argv[1] : "10001";
    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filter, strlen(filter));
    assert( rc == 0);
 
    // 4.从发布者那里接收消息, 接收10条自己想要的数据
    int update_nbr;
    long total_temp = 0;
    for(update_nbr = 0; update_nbr < 10; update_nbr++)
    {
        // 5.接收数据
        char *string = s_recv(subscriber);
        assert(string != NULL);
 
        // 6.将数据中的邮政编码、温度、适度分别存储变量中
        int zipcode, temperature, relhumidity;
        sscanf(string, "%d %d %d", &zipcode, &temperature, &relhumidity);
        total_temp += temperature;
        free(string);
    }
 
    // 7.接收完成之后, 打印一下平均温度
    printf("Average tempature for zipcode '%s' was %dF\n", filter, (int)(total_temp / update_nbr));
 
    // 8.关闭套接字、销毁上下文
    zmq_close(subscriber);
    zmq_ctx_destroy(context);
 
    return 0;
}
 
char *s_recv(void *socket)
{
    // 创建zmq_msg_t对象接收数据
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    int size = zmq_msg_recv(&msg, socket, 0);
    if(size == -1)
    {
        return NULL;
    }
 
    // 将zmq_msg_t对象中的数据保存到字符串中
    char *string = (char*)malloc(size + 1);
    memcpy(string, zmq_msg_data(&msg), size);
 
    zmq_msg_close(&msg);
    string[size] = 0;
 
    return string;
}
