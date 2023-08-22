/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 12:01:06 AM
 * =====================
 * 인터 통신을 위해 접속한 인증,로비,게임 서버 세션
 */


#pragma once

#include <JNetwork/Host/TcpSession.h>
#include <SteinsGate/Common/ServerEnum.h>

class CenterSession : public JNetwork::TcpSession
{
public:
	struct BootState
	{
		ServerType_t ServerType;
		ServerBootState_t State;
	};

	CenterSession(
		JNetwork::TcpServer* server,
		const JNetwork::IOCPPtr& iocp,
		const JCore::MemoryPoolAbstractPtr& bufferAllocator,
		int recvBufferSize,
		int sendBufferSize
	);

	void OnConnected() override;
	void OnDisconnected() override;

	InterServerClientType_t GetClientType() const { return m_eClientType; }
	Int8 GetServerId() const { return m_iServerId; }

	void SetClientType(InterServerClientType_t type) { m_eClientType = type; }
	void SetClientInformation(InterServerClientType_t type, Int8 serverId);
	bool IsValid() const;
private:
	InterServerClientType_t m_eClientType;
	Int8 m_iServerId;
};


