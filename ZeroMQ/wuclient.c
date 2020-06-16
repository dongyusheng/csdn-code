//wuclient.c
//https://blog.csdn.net/qq_41453285/article/details/106005939
//https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/wuclient.c
#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

static char* s_recv(void* sokcet);

int main(int argc, char *argv[])
{
    //创建新的上下文
    void* context = zmq_ctx_new();

    //创建SUB套接字、连接服务端
    printf("Collecting updates from weather server...\n");
    void* subscriber = zmq_socket(context, ZMQ_SUB);
    int rc = zmq_connect(subscriber, "tcp://localhost:5556");
    assert(rc == 0);

    //订阅邮政编码，默认为10001(NYC)
    char* filter = (argc > 1)? argv[1] : "10001";
    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filter, strlen(filter));
    assert(rc == 0);

    //处理100个更新
    int update_nbr;
    long total_temp = 0;
    for(update_nbr = 0; update_nbr < 100; update_nbr++)
    {
        //接收订阅消息
        char* string = s_recv(subscriber);

        int zipcode, temperature, relhumidity;
        sscanf(string, "%d %d %d", &zipcode, &temperature, &relhumidity);
        total_temp += temperature;
        free(string);
    }
    //打印平均温度
    printf("Average temperature for zipcode '%s' was %dF\n", filter, (int)(total_temp / update_nbr));

    //善后处理
    zmq_close(subscriber);
    zmq_ctx_term(context);
    return 0;
}

//从套接字接收ZeroMQ字符串，并将其转换为C/C++字符串
static char* s_recv(void* socket)
{
    zmq_msg_t message;
    zmq_msg_init(&message);
 
    int size = zmq_msg_recv(&message, socket, 0);
    if(size == -1)
        return NULL;
 
    char *string = (char*)malloc(size + 1);
    memcpy(string, zmq_msg_data(&message), size);
 
    zmq_msg_close(&message);
    string[size] = 0;
    return string;
}
