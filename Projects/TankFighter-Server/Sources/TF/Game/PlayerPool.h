/*
 * 작성자 : 윤정도
 * 통신 진행중인 Player 포인터를 삭제해버리면 NullPointerException이 발생할 수 있기 때문에
 * 풀을 만들어서 제어가 가능하도록 하자.
 */
#pragma once

#include <JCore/Container/LinkedList.h>
#include <TF/Game/Player.h>

namespace JNetwork { class TcpSession; }

class PlayerPool
{
	PlayerPool() {}
public:
	static PlayerPool* GetInstance();

	bool Initialize(int size);
	bool Finalize();

	// 풀에서 사용할 플레이어에 세션을 세팅해서 반환받는다.
	Player* PopPlayer(JNetwork::TcpSession* session);

	// 연결이 종료된 플레이어 정보를 풀에 다시 돌려놓는다.
	void ReleasePlayer(Player* releasePlayer);
	int Count();
private:
	JCore::LinkedList<Player*> m_PlayerPool;
	JCore::CriticalSectionMutex m_Mutex;

	inline static PlayerPool* ms_pInstance = nullptr;
};

