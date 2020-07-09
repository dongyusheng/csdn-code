#include<stdio.h>  
#include<stdlib.h>  
#include<unistd.h>  
#include<errno.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
#include<string.h>  
#include "udp-piece.h"

#define MYPORT 8887  

/*
* 编译：gcc -o udp-piece-server_ex udp-piece-server.c circular_buffer.c udp-piece.c
*/
#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)  

void echo_server(int sock)  
{  
    char recvbuf[1024] = {0};  
    struct sockaddr_in peeraddr;  
    socklen_t peerlen;  
    int n;  
	int ret;
    udp_piece_t *udp_piece;
	// 初始化
	udp_piece = udp_piece_init(64*1024);
    while (1)  
    {  
        peerlen = sizeof(peeraddr);  
        memset(recvbuf, 0, sizeof(recvbuf));  
        n = recvfrom(sock, recvbuf, sizeof(recvbuf), 0,  
                     (struct sockaddr *)&peeraddr, &peerlen);  
        if (n <= 0)  
        {  
            if (errno == EINTR)  
                continue;  
            
            ERR_EXIT("recvfrom error");  
        }  
        else if(n > 0)  
        {  
           //printf("udp_piece_merge\n");
			ret = udp_piece_merge_ex(udp_piece, recvbuf, n);
			if(ret == 1)
			{
				printf("接收到的数据：%s, 长度 = %d\n\n\n",udp_piece->recv_buf, udp_piece->recv_len);
				udp_piece_reset(udp_piece);
				//printf("reset ret = %d, n = %d\n", ret, n);
			}
			else if(ret == -1)
			{
				printf("分片重组异常\n");
				udp_piece_reset(udp_piece);
			}
			else if(ret == 0)
			{
				printf("ret = %d, n = %d\n", ret, n);
			}
			else
			{
				printf("未知的异常情况\n");
				udp_piece_reset(udp_piece);
			}
        }  
		
    }  
    close(sock);  
}  

int main(void)  
{  
    int sock;  
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  
        ERR_EXIT("socket error");  
    
    struct sockaddr_in servaddr;  
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(MYPORT);  
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  
    
    printf("监听%d端口\n",MYPORT);  
    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)  
        ERR_EXIT("bind error");  
    
    echo_server(sock);  
    
    return 0;  
}  
