/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: Darren
 * @Date: 2019-10-29 17:45:11
 * @LastEditors: Darren
 * @LastEditTime: 2019-10-29 17:55:52
 */
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include "msg.pb.h"

#include "ImPduBase.h"
#include "IM.Login.pb.h"
#include "IM.Message.pb.h"

#define READ_BUF_SIZE	2048

#define CHECK_PB_PARSE_MSG(ret) { \
    if (ret == false) \
    {\
        std::cout << "parse pb msg failed.\n";\
        return;\
    }\
}

int sockfd;
int fds[100];
int size = 0;
Im::helloworld msg;
Im::worldhello msj;
 
void init(void);
void start(void);
void destroy(int signo);
void* recv_msg(void* p);
void send_msg_to_all(int id, std::string str);
void handle_login_request(CImPdu* pPdu);
void handle_client_msg_data(CImPdu* pPdu);

int main()
{
	signal(SIGINT, destroy);
	init();
	start();
	return 0;
}
 
void init(void)
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
 
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(2222);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
 
	int res = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
 
	res = listen(sockfd, 100);
}
 
void start(void)
{
	while(1)
	{
		struct sockaddr_in acvAddr;
		socklen_t len = sizeof(acvAddr);
		int resfd = accept(sockfd, (struct sockaddr*)&acvAddr, &len);
		pthread_t pid;
		pthread_create(&pid, 0, recv_msg, &resfd);
		fds[size++] = resfd;
	}
}
 
void destroy(int signo)
{
	close(sockfd);
	exit(0);
}
 
void* recv_msg(void* p)
{
	int tempFd = *(int*)p;
	CSimpleBuffer	m_in_buf;
	
	while(1)
	{
		uint32_t free_buf_len = m_in_buf.GetAllocSize() - m_in_buf.GetWriteOffset();
		if (free_buf_len < READ_BUF_SIZE)
			m_in_buf.Extend(READ_BUF_SIZE);	// 2048

		printf("handle = %u, recv into\n", tempFd);
		int ret = recv(tempFd, m_in_buf.GetBuffer() + m_in_buf.GetWriteOffset(), READ_BUF_SIZE, 0);
		if(ret <= 0) 
		{
			std::cout << "client fd:" << tempFd << " close\n";
			close(tempFd);
			break;
		}
		m_in_buf.IncWriteOffset(ret);		// 真正的数据生效
		CImPdu* pPdu = NULL;
		while ( ( pPdu = CImPdu::ReadPdu(m_in_buf.GetBuffer(), m_in_buf.GetWriteOffset()) ) )
		{
			uint32_t pdu_len = pPdu->GetLength();
            printf("handle = %u, pdu_len into = %u\n",tempFd, pdu_len);
			switch (pPdu->GetCommandId()) {		// 到底属于哪个protobuf
				case IM::BaseDefine::CID_LOGIN_REQ_USERLOGIN:
					handle_login_request(pPdu );
				break;

				case IM::BaseDefine::CID_MSG_DATA:
					handle_client_msg_data(pPdu);
				break;

				default:
					printf("handle = %u, can't handle the pdu, cmd = 0x%x\n",tempFd, pPdu->GetCommandId());
				break;
				
			}

			m_in_buf.Read(NULL, pdu_len); // 100字节，跳过一个pdu
			delete pPdu;
            pPdu = NULL;
		}
		
	}
}

void handle_login_request(CImPdu* pPdu) 
{
	IM::Login::IMLoginReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	std::string user_name = msg.user_name();
    std::string password = msg.password();
	std::string client_version = msg.client_version();
	IM::BaseDefine::ClientType client_type = msg.client_type();
	
	std::cout << "\nhandle_login_request:" << std::endl;
	std::cout << "user_name: " << user_name << std::endl;
	std::cout << "password: " << password << std::endl;
	if(client_type == IM::BaseDefine::CLIENT_TYPE_WINDOWS)
		std::cout << "client_type: " << "CLIENT_TYPE_WINDOWS" << std::endl;
	if(client_type == IM::BaseDefine::CLIENT_TYPE_ANDROID)
		std::cout << "client_type: " << "CLIENT_TYPE_ANDROID" << std::endl;
	std::cout << "client_version: " << client_version << std::endl;
	std::cout << "tails: " << msg.tails() << std::endl;
	
}

void handle_client_msg_data(CImPdu* pPdu)
{
	IM::Message::IMMsgData msg;
	CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
	
	if (msg.msg_data().length() == 0) {
		printf("discard an empty message, uid=%u ", msg.from_user_id());
		return;
	}
	uint32_t to_session_id = msg.to_session_id();
    uint32_t msg_id = msg.msg_id();
	uint8_t msg_type = msg.msg_type();
    std::string msg_data = msg.msg_data();

	
	std::cout << "\nhandle_client_msg_data:" << std::endl;
	std::cout << "to_session_id: " << to_session_id << std::endl;
	std::cout << "msg_id: " << msg_id << std::endl;
	std::cout << "msg_type: " << msg_type << std::endl;
	std::cout << "msg_data: " << msg_data << std::endl;

}


 
void send_msg_to_all(int id, std::string str)
{
	char sendbuf[1024] = {0};
	std::string data;
	msj.set_id(id);
	msj.set_str(str);
	msj.SerializeToString(&data);
	strcpy(sendbuf, data.c_str());
	int i = 0;
	for(i = 0; i < size; i++)
	{
		if(0 != fds[i])
		{
			send(fds[i], sendbuf, strlen(sendbuf), 0);
		}
	}
}
