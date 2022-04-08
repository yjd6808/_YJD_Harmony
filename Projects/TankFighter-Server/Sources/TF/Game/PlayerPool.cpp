/*
 * 작성자 : 윤정도
 */

#include <TF/PrecompiledHeader.h>
#include <TF/Game/PlayerPool.h>
#include <TF/Util/Console.h>

using namespace JCore;
using namespace JNetwork;

PlayerPool* PlayerPool::GetInstance() {
	if (ms_pInstance == nullptr) {
		ms_pInstance = new PlayerPool;
	}

	return ms_pInstance;
}

bool PlayerPool::Initialize(int size) {
	for (int i = 0; i < size; i++) {
		Player* pNewPlayer = new (std::nothrow) Player();

		if (pNewPlayer == nullptr) {
			return false;
		}

		m_PlayerPool.PushBack(pNewPlayer);
	}

	Console::WriteLine("플레이어 풀 %d 초기화완료", size);
	return true;
}

bool PlayerPool::Finalize() {
	m_PlayerPool.Extension().ForEach([](Player* p) { delete  p; });
	Console::WriteLine("플레이어 풀 비우기 완료");
	return true;
}

Player* PlayerPool::PopPlayer(TcpSession* session) {
	CriticalSectionLockGuard guard(m_Mutex);

	if (m_PlayerPool.IsEmpty()) {
		return new Player(session);
	}


	Player* pop = m_PlayerPool.Front();
	m_PlayerPool.PopFront();
	pop->m_pSession = session;
	return pop;
}


void PlayerPool::ReleasePlayer(Player* releasePlayer) {
	releasePlayer->Initialize();
	CriticalSectionLockGuard guard(m_Mutex);
	m_PlayerPool.PushBack(releasePlayer);
}

int PlayerPool::Count() {
	CriticalSectionLockGuard guard(m_Mutex);
	return m_PlayerPool.Size();
}
