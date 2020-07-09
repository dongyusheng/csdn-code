#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <errno.h>  
#include <string.h>  
#include "udp-piece.h"

/*
* compile ：gcc -o udp-piece-client udp-piece-client.c  udp-piece.c circular_buffer.c
*/

#define MYPORT 8887  
char* SERVERIP = "127.0.0.1";  

// 要发送的数据
char test_str[] = {"6c7166407562756e74753a2f6d6e742f686766732f776f726b24206364206c69626576656e742f6578616d706c652fa6c7\n	\
	166407562756e74753a2f6d6e742f686766732f776f726b2f6c69626576656e742f6578616d706c6524206c73a312d636c69656e7420202020\n	\
	322d636c69656e7420202020332d636c69656e74202020206331306b5f7365727665722e632020202020202020202020202020202020202020\n	\
	20207468726561645f706f6c6c5f636c69656e742e63202074696d65722e63a312d636c69656e742e632020322d636c69656e742e632020332\n	\
	d636c69656e742e6320206c69626576656e742d74687265616420202020202020202020202020202020202020207468726561645f706f6c6c5\n	\
	f736572766572a312d73657276657220202020322d73657276657220202020332d736572766572202020206c69626576656e742d7468726561\n	\
	642d32303134303232342d312e7461722e677a20207468726561645f706f6c6c5f7365727665722e63a312d7365727665722e632020322d736\n	\
	5727665722e632020332d7365727665722e6320207468726561645f706f6c6c5f636c69656e742020202020202020202020202020202020746\n	\
	96d6572a6c7166407562756e74753a2f6d6e742f686766732f776f726b2f6c69626576656e742f6578616d706c652420676363202d6f206331\n	\
	306b5f736572766572206331306b5f7365727665722e6320a6c7166407562756e74753a2f6d6e742f686766732f776f726b2f6c69626576656\n	\
	e742f6578616d706c6524206c73a312d636c69656e7420202020322d636c69656e7420202020332d636c69656e74202020206331306b5f7365\n	\
	727665722020202020202020202020202020202020202020202020207468726561645f706f6c6c5f636c69656e742020202074696d6572a312\n	\
	d636c69656e742e632020322d636c69656e742e632020332d636c69656e742e6320206331306b5f7365727665722e632020202020202020202\n	\
	02020202020202020202020207468726561645f706f6c6c5f636c69656e742e63202074696d65722e63a312d73657276657220202020322d73\n	\
	657276657220202020332d736572766572202020206c69626576656e742d746872656164202020202020202020202020202020202020202074\n	\
	68726561645f706f6c6c5f736572766572a312d7365727665722e632020322d7365727665722e632020332d7365727665722e6320206c69626\n	\
	576656e742d7468726561642d32303134303232342d312e7461722e677a20207468726561645f706f6c6c5f7365727665722e63a6c71664075\n	\
	62756e74753a2f6d6e742f686766732f776f726b2f6c69626576656e742f6578616d706c6524202e2f6331306b5f736572766572a65706f6c6\n	\
	c73657276657220737461727475702c706f"};

#define ERR_EXIT(m) 	\
    do{ \
        perror(m); 	\
        exit(EXIT_FAILURE); 	\
    } while(0)  

void echo_client(int sock)  {  
    struct sockaddr_in servaddr;  
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(MYPORT);  
    servaddr.sin_addr.s_addr = inet_addr(SERVERIP);  
    
    int ret;  
	int len;
	int pieces;
	int send_len = 0;
	
    //while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) 
	// 初始化
    udp_piece_t *udp_piece = udp_piece_init(64*1024);
	while(1)		//
	{  
		len = sizeof(test_str);
        
        printf("\n向服务器发送数据长度：%d\n", len);  

		// 进行分片
		pieces = udp_piece_cut(udp_piece, test_str, len);
		printf("pieces = %d\n", pieces);
		// 逐片发送
		for(int i = 0; i < pieces; i++)
		{
			char *buf;
			int size;
			buf = udp_piece_get(udp_piece, i, &size);
			printf("向服务器发送分片[%d]长度：%d, buf = %p\n", i*2, 5, buf);  
			//send_len = sendto(sock, buf, size, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)); 
			sendto(sock, buf, 5, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)); 
			printf("向服务器发送分片[%d]长度：%d, buf = %p\n", i*2+1, size - 5, buf);
			sendto(sock, buf + 5, size - 5, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)); 
			if(send_len != size)
			{
				//printf("An error occurred in this call of  sendto, need send len = %d, but actual len = %d\n",
				//	size, send_len);
			}
		}

		sleep(2);
    }  
    
    close(sock);  
    
    
}  

int main(void)  
{  
    int sock;  
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  
        ERR_EXIT("socket");  
    
    echo_client(sock);  
    
    return 0;  
}  
