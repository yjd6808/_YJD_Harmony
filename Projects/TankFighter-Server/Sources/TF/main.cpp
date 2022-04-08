#include <TF/PrecompiledHeader.h>
#include <TF/Host/GameServer.h>
#include <TF/Util/Console.h>
#include <TF/Database/MysqlDatabase.h>
#include <TF/Game/PlayerPool.h>
#include <TF/Game/World.h>
#include <TF/Worker/SystemWorker.h>

#include <JNetwork/Winsock.h>
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
		MemoryLeakDetector detector;

		detector.StartDetect();

		GameServer* pGameServer = GameServer::GetInstance();
		MysqlDatabase* pDatabase = MysqlDatabase::GetInstance();
		PlayerPool* pPlayerPool = PlayerPool::GetInstance();
		World* pWorld = World::GetInstance();
		SystemWorker* pSystemWorker = SystemWorker::GetInstance();

		// ���� �ʱ�ȭ
		if (!Winsock::Initialize(2, 2)) {
			return ABNORMAL_EXIT_WINSOCK_INIT_FAIED;
		}

		// �����ͺ��̽� �ʱ�ȭ
		if (!pDatabase->Initialize()) {
			return ABNORMAL_EXIT_DATABASE_INIT_FAIED;
		}

		// �÷��̾� Ǯ �ʱ�ȭ
		if (!pPlayerPool->Initialize(PLAYER_POOL_SIZE)) {
			return ABNORMAL_EXIT_PLAYERPOOL_INIT_FAILED;
		}
		
		// ���� �ʱ�ȭ
		if (!pWorld->Initialize()) {
			return ABNORMAL_EXIT_WORLD_INIT_FAILED;
		}

		// ���� ���� ����
		if (!pGameServer->StartServer()) {
			return ABNORMAL_EXIT_GAMESERVER_START_FAILED;
		}

		// �ý��� Ŀ�ǵ� �Է��� �ޱ� ���� 
		pSystemWorker->Run();

		// ======================================
		WaitForSingleObject(pSystemWorker->CreateExitHandle(), INFINITE);	

		if (!pGameServer->Stop()) {
			return ABNORMAL_EXIT_GAMESERVER_STOP_FAILED;
		}

		// �÷��̾� Ǯ ����
		if (!pPlayerPool->Finalize()) {
			return ABNORMAL_EXIT_PLAYERPOOL_FINALIZE_FAILED;
		}
		
		// �����ͺ��̽� ����
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
		DeleteSafe(pSystemWorker);
		DeleteSafe(pDatabase);
		DeleteSafe(pPlayerPool);
		DeleteSafe(pWorld);
		DeleteSafe(pGameServer);

		// �޸� ���� Ȯ��
		const int iLeakMemory = detector.StopDetect();
		if (iLeakMemory == 0) {
			Console::WriteLine(ConsoleColor::LIGHTGREEN, "[�����մϴ�!] �޸� ������ ���׿� ���!!!!!!!");
			Console::WriteLine(ConsoleColor::LIGHTGREEN, "[�����մϴ�!] �� ���ϼ̾�� ���� (��ȭ����)");
		} else {
			Console::WriteLine(ConsoleColor::LIGHTMAGENTA, "[�̷�..] %d ����Ʈ ��ŭ ������ �־��! �� ��ġ����.\n", iLeakMemory);
		}
		Console::SetColor(ConsoleColor::LIGHTGRAY);
	}
	return 0;
}
