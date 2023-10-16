/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 8:19:04 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/TcpSession.h>
#include <TF/Server/Contents/Player.h>

class GameSession : public JNetwork::TcpSession
{
public:
	GameSession(
		JNetwork::TcpServer* server,
		const JNetwork::IOCPPtr& iocp,
		const JCore::MemoryPoolAbstractPtr& bufferAllocator
	);

	JCORE_GETTER_SETTER(U, Player*, Player, m_pPlayer)
private:
	Player* m_pPlayer;
};



