#pragma once

#include <exception>

/*
	�������� TCP/IP �н��� ���� �ڵ��Դϴ�.

	���ϸ� : JStudyServerBase.h
	�ۼ��� : ������
	�ۼ��� : 22/01/03(��) ���� 10��

	Ŭ���� ����
	 ���� Ŭ������ �ֻ��� �θ� Ŭ�����Դϴ�.
	 �� Ŭ������ ��ӹ޾� TcpServer�� UdpServer Ŭ������ �����ϵ��� �մϴ�.
*/

namespace JStudy {

class ServerBase
{
protected:
	virtual void Initialize();
	virtual void Open() = 0;
	virtual void Close() = 0;
};

} // namespace JStudy

