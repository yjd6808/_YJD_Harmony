#define _WINSOCKAPI_		// winsock.h 연속 include 방지

#include <JNetwork/Winsock.h>
#include <TF/Host/GameServer.h>
#include <TF/Util/Console.h>
#include <TF/Database/MysqlDatabase.h>
#include <TF/Game/World.h>

#include "Game/PlayerPool.h"
#include <Common/MemoryLeakDetector.h>


using namespace JNetwork;

// 비정상 종료 반환값들
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
	
	
	// 콘솔창 출력 동기화
	Winsock::SetMutex(&mtx);
	Console::SetMutex(&mtx);

	{
		GameServer* pGameServer = GameServer::GetInstance();
		MysqlDatabase* pDatabase = MysqlDatabase::GetInstance();
		PlayerPool* pPlayerPool = PlayerPool::GetInstance();
		World* pWorld = World::GetInstance();


		// 윈속 초기화
		if (!Winsock::Initialize(2, 2)) {
			return ABNORMAL_EXIT_WINSOCK_INIT_FAIED;
		}

		// 데이터베이스 초기화
		if (!pDatabase->Initialize()) {
			return ABNORMAL_EXIT_DATABASE_INIT_FAIED;
		}

		// 플레이어 풀 초기화
		if (!pPlayerPool->Initialize(25)) {
			return ABNORMAL_EXIT_PLAYERPOOL_INIT_FAILED;
		}

		// 월드 초기화
		// 문제 없음
		if (!pWorld->Initialize()) {
			return ABNORMAL_EXIT_WORLD_INIT_FAILED;
		}


		// 게임 서버 시작
		if (!pGameServer->StartServer()) {
			return ABNORMAL_EXIT_GAMESERVER_START_FAILED;
		}



		// c 입력시 종료
		while (getchar() != 'c') {}

		if (!pGameServer->Stop()) {
			return ABNORMAL_EXIT_GAMESERVER_STOP_FAILED;
		}

		if (!pPlayerPool->Finalize()) {
			return ABNORMAL_EXIT_PLAYERPOOL_FINALIZE_FAILED;
		}

		// 데이터베이스 초기화
		if (!pDatabase->Finalize()) {
			return ABNORMAL_EXIT_DATABASE_FINALIZE_FAIED;
		}

		if (!pWorld->Finalize()) {
			return ABNORMAL_EXIT_WORLD_FINALIZE_FAILED;
		}

		if (!Winsock::Finalize()) {
			return ABNORMAL_EXIT_WINSOCK_FINALIZE_FAIED;
		}

		// 제거 순서를 지키기 위해 수동으로 삭제해주도록 하자.
		DeleteSafe(pDatabase);
		DeleteSafe(pPlayerPool);
		DeleteSafe(pWorld);
		DeleteSafe(pGameServer);
	}
	return 0;
}
