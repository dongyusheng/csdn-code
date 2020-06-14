/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: Darren
 * @Date: 2019-10-29 17:45:10
 * @LastEditors: Darren
 * @LastEditTime: 2019-10-29 17:57:34
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include "msg.pb.h"
#include <iostream>

#include "ImPduBase.h"
#include "IM.Login.pb.h"
#include "IM.Message.pb.h"
 
 
int sockfd;
Im::helloworld msg;
Im::worldhello msj;
void init(void);
void start(void);
void destroy(int signo);
void* recv_msg(void* p);

int s_seq_no = 0;
 
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
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(2222);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
 
	int res = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
}
 
void start(void)
{
	uint32_t login_msg = 0;
	int num;
	std::string str;
	std::string data;
	pthread_t pid;
	pthread_create(&pid, 0, recv_msg, 0);
	while(1)
	{
		CImPdu cPdu;
		if(login_msg++ % 2 == 0) 
		{
			// 发送登录信息
			IM::Login::IMLoginReq msg;
		    msg.set_user_name("liaoqingfu");
		    msg.set_password("123456");
		    msg.set_online_status(IM::BaseDefine::USER_STATUS_ONLINE);
		    msg.set_client_type(IM::BaseDefine::CLIENT_TYPE_WINDOWS);
		   	msg.set_client_version("1.0");
			msg.set_tails("iphone11pro");
			
		    cPdu.SetPBMsg(&msg);	// 把protobuf序列化后的数据填充到data段
		    cPdu.SetServiceId(IM::BaseDefine::SID_LOGIN);
		    cPdu.SetCommandId(IM::BaseDefine::CID_LOGIN_REQ_USERLOGIN);
			
		    uint32_t nSeqNo = s_seq_no++;
		    cPdu.SetSeqNum(nSeqNo);

			std::cout << "send login Pdu len = " << cPdu.GetLength() << std::endl;
			send(sockfd, cPdu.GetBuffer(), cPdu.GetLength(), 0);
		}
	    else if(login_msg++ % 3 == 0) 	// 发送文本消息
		{
			IM::Message::IMMsgData msg;
		    msg.set_from_user_id(123);
		    msg.set_to_session_id(67);
		    msg.set_msg_id(0);
		    msg.set_create_time(time(NULL));
		    msg.set_msg_type(IM::BaseDefine::MSG_TYPE_SINGLE_TEXT);
		    msg.set_msg_data("今天主要讲protobuf");
			
		    cPdu.SetPBMsg(&msg);
		    cPdu.SetServiceId(IM::BaseDefine::SID_MSG);
		    cPdu.SetCommandId(IM::BaseDefine::CID_MSG_DATA);
		    uint32_t nSeqNo = s_seq_no++;
		    cPdu.SetSeqNum(nSeqNo);
			std::cout << "send msg Pdu len = " << cPdu.GetLength() << std::endl;
			send(sockfd, cPdu.GetBuffer(), cPdu.GetLength(), 0);
		} else
		{
			//IM::Message::IMMsgDataAck msgAck;
			//msgAck.set_user_id(123);
		}

		sleep(5);
	}
}
 
void destroy(int signo)
{
	std::cout << "close..." << std::endl;
	close(sockfd);
	sleep(1);
	exit(0);
}
void* recv_msg(void* p)
{
	while(1)
	{
		char recvbuf[1025] = {0};
		recv(sockfd, recvbuf, 1024, 0);
		std::string data = recvbuf;
		msj.ParseFromString(data);
		std::cout << msj.id() << std::endl;
		std::cout << msj.str() << std::endl;
	}
}

