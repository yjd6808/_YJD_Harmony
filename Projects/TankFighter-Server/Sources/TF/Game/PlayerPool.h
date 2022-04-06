/*
 * �ۼ��� : ������
 * ��� �������� Player �����͸� �����ع����� NullPointerException�� �߻��� �� �ֱ� ������
 * Ǯ�� ���� ��� �����ϵ��� ����.
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

	// Ǯ���� ����� �÷��̾ ������ �����ؼ� ��ȯ�޴´�.
	Player* PopPlayer(JNetwork::TcpSession* session);

	// ������ ����� �÷��̾� ������ Ǯ�� �ٽ� �������´�.
	void ReleasePlayer(Player* releasePlayer);
	int Count();
private:
	JCore::LinkedList<Player*> m_PlayerPool;
	JCore::CriticalSectionMutex m_Mutex;

	inline static PlayerPool* ms_pInstance = nullptr;
};

