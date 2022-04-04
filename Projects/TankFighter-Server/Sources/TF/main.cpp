#define _WINSOCKAPI_		// winsock.h ���� include ����

#include <JNetwork/Winsock.h>
#include <TF/Host/GameServer.h>
#include <TF/Util/Console.h>
#include <TF/Database/MysqlDatabase.h>
#include <TF/Game/World.h>


using namespace JNetwork;

// ������ ���� ��ȯ����
#define ABNORMAL_EXIT_WINSOCK_INIT_FAIED		-0x01;
#define ABNORMAL_EXIT_DATABASE_INIT_FAIED		-0x02;
#define ABNORMAL_EXIT_WORLD_INIT_FAILED			-0x06;
#define ABNORMAL_EXIT_GAMESERVER_START_FAILED	-0x03;
#define ABNORMAL_EXIT_GAMESERVER_STOP_FAILED	-0x04;

int main() {

	JCore::CriticalSectionMutex mtx;

	// �ܼ�â ��� ����ȭ
	Winsock::SetMutex(&mtx);
	Console::SetMutex(&mtx);

	MysqlDatabase* pDatabase = MysqlDatabase::GetInstance();
	GameServer* pGameServer = GameServer::GetInstance();
	World* pWorld = World::GetInstance();

	// ���� �ʱ�ȭ
	if (!Winsock::Initialize(2, 2)) {
		return ABNORMAL_EXIT_WINSOCK_INIT_FAIED;
	}

	// �����ͺ��̽� �ʱ�ȭ
	if (!pDatabase->Initialize()) {
		return ABNORMAL_EXIT_DATABASE_INIT_FAIED;
	}

	// ���� �ʱ�ȭ
	if (!pWorld->Initialize()) {
		return ABNORMAL_EXIT_WORLD_INIT_FAILED;
	}

	// ���� ���� ����
	if (!pGameServer->StartServer()) {
		return ABNORMAL_EXIT_GAMESERVER_START_FAILED;
	}

	

	// c �Է½� ����
	while (getchar() != 'c') {}

	if (pGameServer->Stop()) {
		return ABNORMAL_EXIT_GAMESERVER_STOP_FAILED;
	}

	if (!Winsock::Finalize()) {
		return -4;
	}

	// ���� ������ ��Ű�� ���� �������� �������ֵ��� ����.
	DeleteSafe(pDatabase);
	DeleteSafe(pGameServer);

	return 0;
}
