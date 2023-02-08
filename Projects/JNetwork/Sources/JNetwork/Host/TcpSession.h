/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 11:23:59 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>

NS_JNET_BEGIN

class TcpServer;
class TcpSession : public Session
{
public:
	TcpSession(TcpServer* server, const IOCPPtr& iocp, int recvBufferSize, int sendBufferSize);

	void NotifyCommand(ICommand* cmd) override;

	void Connected() override;
	void Disconnected() override;
	bool AcceptAsync();
	void AcceptWait();													// 미리 생성된 TcpSession이 AcceptEx를 호출하기 직전에 호출될 함수 - 연결 수락 대기상태
	bool Accepted(Int32UL receivedBytes);		// 서버가 해당세션의 연결을 수락시 최종적으로 호출될 함수
	void Sent(ISendPacket* sentPacket, Int32UL receivedBytes) override;
private:
	TcpServer* m_pServer;
};


NS_JNET_END
