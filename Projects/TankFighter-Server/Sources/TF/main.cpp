#define _WINSOCKAPI_		// winsock.h ���� include ����

#include <JNetwork/Winsock.h>
#include <TF/Host/GameServer.h>
#include <TF/Util/Console.h>
#include <TF/Database/MysqlDatabase.h>
#include <TF/Game/World.h>

#include "Game/PlayerPool.h"
#include <Common/MemoryLeakDetector.h>


using namespace JNetwork;

// ������ ���� ��ȯ����
#define ABNORMAL_EXIT_WINSOCK_INIT_FAIED			-0x01;
#define ABNORMAL_EXIT_WINSOCK_FINALIZE_FAIED		-0x0a;
#define ABNORMAL_EXIT_DATABASE_INIT_FAIED			-0x02;
#define ABNORMAL_EXIT_DATABASE_FINALIZE_FAIED		-0x0b;
#define ABNORMAL_EXIT_WORLD_INIT_FAILED				-0x06;
#define ABNORMAL_EXIT_WORLD_FINALIZE_FAILED			-0x08;
#define ABNORMAL_EXIT_PLAYERPOOL_INIT_FAILED		-0x07;
#define ABNORMAL_EXIT_PLAYERPOOL_FINALIZE_FAILED	-0x09;
#define ABNORMAL_EXIT_GAMESERVER_START_FAILED		-0x03;
#define ABNORMAL_EXIT_GAMESERVER_STOP_FAILED		-0x04;


int main() {

	JCore::CriticalSectionMutex mtx;
	
	
	// �ܼ�â ��� ����ȭ
	Winsock::SetMutex(&mtx);
	Console::SetMutex(&mtx);

	{
		GameServer* pGameServer = GameServer::GetInstance();
		MysqlDatabase* pDatabase = MysqlDatabase::GetInstance();
		PlayerPool* pPlayerPool = PlayerPool::GetInstance();
		World* pWorld = World::GetInstance();


		// ���� �ʱ�ȭ
		if (!Winsock::Initialize(2, 2)) {
			return ABNORMAL_EXIT_WINSOCK_INIT_FAIED;
		}

		// �����ͺ��̽� �ʱ�ȭ
		if (!pDatabase->Initialize()) {
			return ABNORMAL_EXIT_DATABASE_INIT_FAIED;
		}

		// �÷��̾� Ǯ �ʱ�ȭ
		if (!pPlayerPool->Initialize(25)) {
			return ABNORMAL_EXIT_PLAYERPOOL_INIT_FAILED;
		}

		// ���� �ʱ�ȭ
		// ���� ����
		if (!pWorld->Initialize()) {
			return ABNORMAL_EXIT_WORLD_INIT_FAILED;
		}


		// ���� ���� ����
		if (!pGameServer->StartServer()) {
			return ABNORMAL_EXIT_GAMESERVER_START_FAILED;
		}



		// c �Է½� ����
		while (getchar() != 'c') {}

		if (!pGameServer->Stop()) {
			return ABNORMAL_EXIT_GAMESERVER_STOP_FAILED;
		}

		if (!pPlayerPool->Finalize()) {
			return ABNORMAL_EXIT_PLAYERPOOL_FINALIZE_FAILED;
		}

		// �����ͺ��̽� �ʱ�ȭ
		if (!pDatabase->Finalize()) {
			return ABNORMAL_EXIT_DATABASE_FINALIZE_FAIED;
		}

		if (!pWorld->Finalize()) {
			return ABNORMAL_EXIT_WORLD_FINALIZE_FAILED;
		}

		if (!Winsock::Finalize()) {
			return ABNORMAL_EXIT_WINSOCK_FINALIZE_FAIED;
		}

		// ���� ������ ��Ű�� ���� �������� �������ֵ��� ����.
		DeleteSafe(pDatabase);
		DeleteSafe(pPlayerPool);
		DeleteSafe(pWorld);
		DeleteSafe(pGameServer);
	}
	return 0;
}
