#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>

#define HTTP_VERSION "HTTP/1.1"
#define CONNECTION_TYPE "Connection:close"

#define HOSTNAME_LENGTH 1024
#define BUFFER_SIZE 1024
#define EPOLL_WAIT_NUM 10


typedef void (*async_result_cb)(const char *hostname, char *result);

struct async_context {
    int epoll_fd;
    pthread_t thread_id;
};

struct ep_arg {
    int client_fd;
    char hostname[HOSTNAME_LENGTH];
    async_result_cb cb;
};


struct http_request {
    const char *hostname;
    const char *resource;
};

struct http_request reqs[] = {
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=beijing&language=zh-Hans&unit=c" },
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=changsha&language=zh-Hans&unit=c" },
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=shenzhen&language=zh-Hans&unit=c" },
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=shanghai&language=zh-Hans&unit=c" },
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=tianjin&language=zh-Hans&unit=c" },
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=wuhan&language=zh-Hans&unit=c" },
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=hefei&language=zh-Hans&unit=c" },
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=hangzhou&language=zh-Hans&unit=c" },
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=nanjing&language=zh-Hans&unit=c" },
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=jinan&language=zh-Hans&unit=c" },
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=taiyuan&language=zh-Hans&unit=c" },
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=wuxi&language=zh-Hans&unit=c" },
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=suzhou&language=zh-Hans&unit=c" }
};

/*****************************************************************************
 函 数 名: host_to_ip
 功能描述  : 根据主机名返回对应的ip
 输入参数  : const char *hostname -> 要解析的主机名
 输出参数  : 无
 返 回 值: const char * --> 成功主机名对应的ip地址,失败返回NULL
 调用函数  : 
 被调函数  : 
 
 修改历史:
  1.日    期   : 2020年5月26日
    作    者   : 董雨生
    修改内容   : 新生成函数

*****************************************************************************/
const char *host_to_ip(const char *hostname);

/*****************************************************************************
 函 数 名: async_http_create_socket
 功能描述  : 在内部创建socket,并连接到服务端
 输入参数  : const char* ip --> 服务端ip
 输出参数  : 无
 返 回 值  : int --> 成功返回客户端socket,失败返回-1
 调用函数  : 
 被调函数  : 
 
 修改历史:
  1.日    期   : 2020年5月26日
    作    者   : 董雨生
    修改内容   : 新生成函数

*****************************************************************************/
int async_http_create_socket(const char* ip);

/*****************************************************************************
 函 数 名: async_http_init_context
 功能描述  : 初始化上下文
 输入参数  : 无
 输出参数  : 无
 返 回 值: struct async_context * --> 成功创建的当前程序的上下文,失败返回NULL
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2020年5月27日
    作    者   : 董雨生
    修改内容   : 新生成函数

*****************************************************************************/
struct async_context *async_http_init_context();

/*****************************************************************************
 函 数 名: async_http_thread_func
 功能描述  : 线程执行函数
 输入参数  : void *arg  
 输出参数  : 无
 返 回 值: void *
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2020年5月27日
    作    者   : 董雨生
    修改内容   : 新生成函数

*****************************************************************************/
void *async_http_thread_func(void *arg);

/*****************************************************************************
 函 数 名: async_http_commit
 功能描述  : 执行一次HTTP请求的操作
 输入参数  : const char *hostname --> 请求的服务端的主机名
           const char* resource --> HTTP请求的资源
           struct async_context *ctx --> 上下文
           async_result_cb cb --> 客户端回调函数
 输出参数  : 无
 返 回 值: int --> 成功返回0,失败返回-1
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2020年5月27日
    作    者   : 董雨生
    修改内容   : 新生成函数

*****************************************************************************/
int async_http_commit(const char *hostname, const char* resource, struct async_context *ctx, async_result_cb cb);

/*****************************************************************************
 函 数 名: async_http_client_result_callback
 功能描述  : 客户端回调函数
 输入参数  : const char *hostname --> 客户端连接的服务端的主机名
           char *result --> 服务端返回的数据集
 输出参数  : 无
 返 回 值: void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2020年5月27日
    作    者   : 董雨生
    修改内容   : 新生成函数

*****************************************************************************/
void async_http_client_result_callback(const char *hostname, char *result);

/*****************************************************************************
 函 数 名: async_http_destory_context
 功能描述  : 关闭上下文，在其内部会关闭epoll套接字和取消线程
 输入参数  : struct async_context *ctx --> 上下文结构体
 输出参数  : 无
 返 回 值: void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2020年5月27日
    作    者   : 董雨生
    修改内容   : 新生成函数

*****************************************************************************/
void async_http_destory_context(struct async_context *ctx);


int main()
{
    //初始化上下文
    struct async_context *context = async_http_init_context();
    if(context == NULL)
        exit(EXIT_FAILURE);

    //进行HTTP请求
    int count = sizeof(reqs) / sizeof(reqs[0]);
    int i;
    for(i = 0; i < count; ++i)
        async_http_commit(reqs[i].hostname, reqs[i].resource, context, async_http_client_result_callback);

    //销毁上下文
    async_http_destory_context(context);
    
    return 0;
}


const char *host_to_ip(const char *hostname)
{
    struct hostent *host_entry = (struct hostent*)malloc(sizeof(struct hostent));
    host_entry = NULL;

    //根据主机名返回对应的struct hostent结构
    host_entry = gethostbyname(hostname);
    if(host_entry == NULL){
        fprintf(stderr, "gethostbyname error: %s\n", hstrerror(h_errno));
        return NULL;
    }
    else{
        //将网络字节序的ip地址转换为本地字节序的ip地址
        return inet_ntoa(*(struct in_addr*)(*host_entry->h_addr_list));
    }
}

int async_http_create_socket(const char* ip)
{
    int sockfd = -1;

    //创建socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    //服务端地址
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(80);

    //连接服务端
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        perror("connect");
        exit(EXIT_FAILURE);
    }

    //将套接字设置为非阻塞的
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    
    return sockfd;
}

struct async_context *async_http_init_context()
{
    struct async_context *context = (struct async_context*)malloc(sizeof(struct async_context));
    if(context == NULL){
        perror("malloc");
        return NULL;
    }
        
    //创建epoll套接字
    int epfd = epoll_create(5);
    if(epfd == -1){
        free(context);
        perror("epoll_create");
        return NULL;
    }
    context->epoll_fd = epfd;
    
    //开启新的线程
    int thread_ret = pthread_create(&context->thread_id, NULL, async_http_thread_func, context);
    if(thread_ret == -1){
        free(context);
        perror("pthread_create");
        return NULL;
    }

    usleep(1);
    return context;
}

void *async_http_thread_func(void *arg)
{
    struct async_context *context = (struct async_context *)arg;

    char buffer[BUFFER_SIZE];
    
    //调用epoll_wait监听
    while(1)
    {
        struct epoll_event events[EPOLL_WAIT_NUM];
        bzero(&events, sizeof(events));

        int epoll_ret = epoll_wait(context->epoll_fd, events, EPOLL_WAIT_NUM, -1);
        if(epoll_ret == -1){
            if(errno == EINTR || errno == EAGAIN)
                continue;
            else{
                perror("epoll_wait");
                break;
            }
        }
        else if(epoll_ret == 0)
            continue;
        for(int i = 0; i < epoll_ret; i++){
            bzero(buffer, BUFFER_SIZE);
            struct ep_arg *data = (struct ep_arg*)events[i].data.ptr;
            int client_fd = data->client_fd;

            if(recv(client_fd, buffer, BUFFER_SIZE, 0) == -1){
                perror("recv");
                close(client_fd);
                free(data);
                continue;
            }
            //调用客户端的回调函数
            data->cb(data->hostname ,buffer);

            //处理完成之后记得将套接字移除
            if(epoll_ctl(context->epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) == -1){
                perror("epoll_ctl");
                close(client_fd);
                free(data);
                continue;
            }

            //善后处理
            close(client_fd);
            free(data);
        }
    }
}

int async_http_commit(const char *hostname, const char* resource, struct async_context *ctx, async_result_cb cb)
{
    const char *ip = host_to_ip(hostname);

    //创建客户端套接字
    int client_fd = async_http_create_socket(ip);

    //向服务端发送数据
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);

    sprintf(buffer, "GET %s %s\r\n\
Host:%s\r\n\
%s\r\n\
\r\n", resource, HTTP_VERSION, hostname, CONNECTION_TYPE);

    printf("%s\n", buffer);
    
    //发送数据
    if(send(client_fd, buffer, strlen(buffer), 0) == -1) {
        perror("send");
        return -1;
    }

    struct ep_arg *data = (struct ep_arg*)calloc(1, sizeof(struct ep_arg));
    data->client_fd = client_fd;
    bcopy(hostname, data->hostname, strlen(hostname));
    data->cb = cb;
    
    //创建事件，加入到epoll池中
    struct epoll_event ev;
    ev.data.ptr = data;
    ev.events = EPOLLIN;

    if(epoll_ctl(ctx->epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1){
        perror("epoll_ctl");
        return -1;
    }

    return 0;
}

void async_http_client_result_callback(const char *hostname, char *result)
{
    printf("%s\n\n\n", result);
    flush(stdout);
}

void async_http_destory_context(struct async_context *ctx)
{
    close(ctx->epoll_fd);
    pthread_cancel(ctx->thread_id);
}
