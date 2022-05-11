/*
 *	�ۼ��� : ������
 *
 *  [���� �޲ٴ� ȸ���� ������ �յ� ������ 22/05/11 ����]
 *	�� ��������� ������ �� �ظ��ϸ� �ֻ�ܿ� ��ġ�ϵ��� �� ��
 *  Windows.h�� �� ���Ϻ��� ������ include �Ǹ� _WINSOCKAPI_ �����ζ��� �浹�߻��� ^^
 *	==> �ƴ� ���� �׷� �ʿ� ����. Windows.h����� include��Ű�� ���ϵ� ���� WIN32_LEAN_AND_MEAN ������ ó�� �ص�
 *
 *	�׽�Ʈ�غ��� �ʹٸ�
 *	TankFighterMain-Client ������Ʈ�� main.cpp �ֻ�ܿ� Windows.h ������ �����ϰ��ִ� ������� �ƹ��ų� include ��Ų ��
 *	�� �Ʒ� �� ��� ���� include �غ� �� �����غ��� �� �� �ִ�!
 */

#pragma once

#pragma comment(lib, "Mswsock.lib")	// AcceptEx ����� ���ؼ� �߰�
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "JCore.lib")


#include <WinSock2.h>
#include <WS2tcpip.h>

#include <sstream>

#define WIN32_LEAN_AND_MEAN			// Windows.h ����� Winsock.h ����� ���ԵǴ� ���� ���´�. �� �ܿ� ������� ���� �߰��Ǵ� ���� ���� // https://social.msdn.microsoft.com/Forums/vstudio/en-US/6177f9bc-db75-4317-9904-02ef55b527d0/linking-problem-with-winsock2?forum=vcgeneral
#include <JCore/Core.h>


#define FORMAT_STR(fmt)  JCore::StringUtil::Format(fmt)
#define MAX_MSS 1460
#define MAX_MTU 1500
#define TEST_DUMMY_PACKET_SIZE	20