#define _WINSOCKAPI_		// winsock.h 연속 include 방지

#include <JNetwork/Winsock.h>
#include <TF/Host/GameServer.h>
#include <TF/Util/Console.h>
#include <TF/Database/MysqlDatabase.h>
#include <TF/Game/World.h>


using namespace JNetwork;

// 비정상 종료 반환값들
#define ABNORMAL_EXIT_WINSOCK_INIT_FAIED		-0x01;
#define ABNORMAL_EXIT_DATABASE_INIT_FAIED		-0x02;
#define ABNORMAL_EXIT_WORLD_INIT_FAILED			-0x06;
#define ABNORMAL_EXIT_GAMESERVER_START_FAILED	-0x03;
#define ABNORMAL_EXIT_GAMESERVER_STOP_FAILED	-0x04;

int main() {

	JCore::CriticalSectionMutex mtx;

	// 콘솔창 출력 동기화
	Winsock::SetMutex(&mtx);
	Console::SetMutex(&mtx);

	MysqlDatabase* pDatabase = MysqlDatabase::GetInstance();
	GameServer* pGameServer = GameServer::GetInstance();
	World* pWorld = World::GetInstance();

	// 윈속 초기화
	if (!Winsock::Initialize(2, 2)) {
		return ABNORMAL_EXIT_WINSOCK_INIT_FAIED;
	}

	// 데이터베이스 초기화
	if (!pDatabase->Initialize()) {
		return ABNORMAL_EXIT_DATABASE_INIT_FAIED;
	}

	// 월드 초기화
	if (!pWorld->Initialize()) {
		return ABNORMAL_EXIT_WORLD_INIT_FAILED;
	}

	// 게임 서버 시작
	if (!pGameServer->StartServer()) {
		return ABNORMAL_EXIT_GAMESERVER_START_FAILED;
	}

	

	// c 입력시 종료
	while (getchar() != 'c') {}

	if (pGameServer->Stop()) {
		return ABNORMAL_EXIT_GAMESERVER_STOP_FAILED;
	}

	if (!Winsock::Finalize()) {
		return -4;
	}

	// 제거 순서를 지키기 위해 수동으로 삭제해주도록 하자.
	DeleteSafe(pDatabase);
	DeleteSafe(pGameServer);

	return 0;
}
