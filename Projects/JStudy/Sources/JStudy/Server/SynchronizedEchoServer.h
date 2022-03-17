#pragma once


/*
	�������� TCP/IP �н��� ���� �ڵ��Դϴ�.

	���ϸ� : JStudySynchronizedEchoServer.hpp
	�ۼ��� : ������
	
	�н� ��ǥ
	1. WSAStartup, bind, listen, accept �Լ��� �н�
	2. ������ �ۼ����� ����
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

