#pragma once


/*
	열혈강의 TCP/IP 학습차 만든 코드입니다.

	파일명 : JStudySynchronizedEchoServer.hpp
	작성자 : 윤정도
	
	학습 목표
	1. WSAStartup, bind, listen, accept 함수의 학습
	2. 서버간 송수신의 구현
*/

#include <JStudy/Server/TcpServer.h>

namespace JStudy {

class SynchronizedEchoServer : public TcpServer
{
public:
	void Open() override;
	void Close() override;
};

}

