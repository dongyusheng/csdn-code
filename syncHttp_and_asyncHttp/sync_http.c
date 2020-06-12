#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>

#define HTTP_VERSION    "HTTP/1.1"
#define ENCODE_TYPE     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"
#define CONNECTION_TYPE "Connection: close"

#define MAX_BUFFER_SIZE 1024

struct http_request {
    const char* hostname;
    const char* resource;
};

struct http_request res[] = {
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=beijing&language=zh-Hans&unit=c" }
    /*{"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=changsha&language=zh-Hans&unit=c" },
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
    {"api.seniverse.com", "/v3/weather/now.json?key=0pyd8z7jouficcil&location=suzhou&language=zh-Hans&unit=c" }*/
};


/*****************************************************************************
 函 数 名: host_to_ip
 功能描述  : 根据主机名返回对应的ip
 输入参数  : const char *hostname -> 要解析的主机名
 输出参数  : 无
 返 回 值: const char * --> 主机名对应的ip地址
 调用函数  : 
 被调函数  : 
 
 修改历史:
  1.日    期   : 2020年5月26日
    作    者   : 董雨生
    修改内容   : 新生成函数

*****************************************************************************/
const char *host_to_ip(const char *hostname);

/*****************************************************************************
 函 数 名: http_create_socket
 功能描述  : 在内部创建socket,并连接到服务端
 输入参数  : const char* ip --> 服务端ip
 输出参数  : 无
 返 回 值  : int --> 客户端的socket
 调用函数  : 
 被调函数  : 
 
 修改历史:
  1.日    期   : 2020年5月26日
    作    者   : 董雨生
    修改内容   : 新生成函数

*****************************************************************************/
int http_create_socket(const char* ip);

/*****************************************************************************
 函 数 名: http_client_commit
 功能描述  : 执行一个http请求
 输入参数  : const char *hostname --> 主机名
           const char *resource --> http请求资源名称
 输出参数  : 无
 返 回 值: void
 调用函数  : 
 被调函数  : 
 
 修改历史:
  1.日    期   : 2020年5月26日
    作    者   : 董雨生
    修改内容   : 新生成函数

*****************************************************************************/
void http_client_commit(const char *hostname, const char *resource);


/*****************************************************************************
 函 数 名: http_send_request
 功能描述  : 向服务端发送请求,并且将服务端的结果作为返回值返回
 输入参数  : int sockfd --> 客户端套接字           
           const char *hostname  --> 服务端主机名
           const char *resource  --> 请求的资源名
 输出参数  : 无
 返 回 值: char * --> 返回服务端回送的数据
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2020年5月26日
    作    者   : 董雨生
    修改内容   : 新生成函数

*****************************************************************************/
char *http_send_request(int sockfd, const char *hostname, const char *resource);

int main()
{
    int size = sizeof(res) / sizeof(res[0]);
    for(int i = 0; i < size; i++)
        http_client_commit(res[i].hostname, res[i].resource);

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

int http_create_socket(const char* ip)
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

void http_client_commit(const char *hostname, const char *resource)
{
    const char *ip = host_to_ip(hostname);

    if(ip == NULL)
        return;

    //创建socket并且连接到服务端
    int sockfd = http_create_socket(ip);

    //发送一条数据
    char *result = http_send_request(sockfd, hostname, resource);

    printf("%s\n", result);
    free(result);
    close(sockfd);
}

char *http_send_request(int sockfd, const char *hostname, const char *resource)
{
    //数据操作缓冲区
    char buffer[MAX_BUFFER_SIZE];
    bzero(buffer, sizeof(buffer));

    //构造HTTP报文
    sprintf(buffer, 
        "GET %s %s\r\n\
Host:%s\r\n\
%s\r\n\
\r\n", resource, HTTP_VERSION, hostname, CONNECTION_TYPE);

    printf("%s\n", buffer);
    //将HTTP报文发送出去
    if(send(sockfd, buffer, strlen(buffer), 0) == -1){
        perror("send");
        exit(EXIT_FAILURE);
    }
    
    //将客户端套接字加入可读字符集中
    fd_set fdread;
    FD_ZERO(&fdread);
    FD_SET(sockfd, &fdread);

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    //用来保存服务端返回的所有结果
    char *result = (char*)malloc(sizeof(int));
    bzero(result, sizeof(result));
    
    //循环监听数据
    while(1)
    {
        int selection = select(sockfd+1, &fdread, NULL, NULL, &tv);
        if(selection == -1){
            perror("select");
            exit(EXIT_FAILURE);
        }
        if(selection == 0)
            continue;
        //如果有数据返回回来，读取数据
        if(FD_ISSET(sockfd, &fdread))
        {
            bzero(buffer, sizeof(buffer));
            //接收数据
            int len = recv(sockfd, buffer, sizeof(buffer), 0);   
            if(len == 0)
                break;
                
            //重新分配空间存放接收到的数据
            result = realloc(result, strlen(result) + len + 1);
            strncat(result, buffer, len);
        }
    }

    return result;
}
