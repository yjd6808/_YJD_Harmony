#include <TF/PrecompiledHeader.h>
#include <TF/Host/GameServer.h>
#include <TF/Database/MysqlDatabase.h>
#include <TF/Game/PlayerPool.h>
#include <TF/Game/World.h>
#include <TF/Worker/SystemWorker.h>

#include <JCore/Utils/Console.h>
#include <JCore/Encoding/CodePage.h>
#include <JCore/Debug/MemoryLeakDetector.h>

#include <JNetwork/Winsock.h>

#ifdef _WIN64
  #error 이 예제는 x86 플랫폼으로만 빌드 가능합니다.
#endif

using namespace JCore;
using namespace JNetwork;

// 비정상 종료 반환값들
#define ABNORMAL_EXIT_WINSOCK_INIT_FAIED			    -0x01;
#define ABNORMAL_EXIT_WINSOCK_FINALIZE_FAIED		  -0x0a;
#define ABNORMAL_EXIT_DATABASE_INIT_FAIED			    -0x02;
#define ABNORMAL_EXIT_DATABASE_FINALIZE_FAIED		  -0x0b;
#define ABNORMAL_EXIT_WORLD_INIT_FAILED				    -0x06;
#define ABNORMAL_EXIT_WORLD_FINALIZE_FAILED			  -0x08;
#define ABNORMAL_EXIT_PLAYERPOOL_INIT_FAILED		  -0x07;
#define ABNORMAL_EXIT_PLAYERPOOL_FINALIZE_FAILED	-0x09;
#define ABNORMAL_EXIT_GAMESERVER_START_FAILED		  -0x03;
#define ABNORMAL_EXIT_GAMESERVER_STOP_FAILED		  -0x04;


int main() {
    SafeConsole::Init();
	SafeConsole::SetOutputCodePage(CodePage::UTF8);

	{
		MemoryLeakDetector detector;
		detector.StartDetect();

		GameServer* pGameServer = GameServer::GetInstance();
		MysqlDatabase* pDatabase = MysqlDatabase::GetInstance();
		PlayerPool* pPlayerPool = PlayerPool::GetInstance();
		World* pWorld = World::GetInstance();
		SystemWorker* pSystemWorker = SystemWorker::GetInstance();

		// 윈속 초기화
		if (!Winsock::Initialize(2, 2)) {
			return ABNORMAL_EXIT_WINSOCK_INIT_FAIED;
		}

		// 데이터베이스 초기화
		if (!pDatabase->Initialize()) {
			return ABNORMAL_EXIT_DATABASE_INIT_FAIED;
		}

		// 플레이어 풀 초기화
		if (!pPlayerPool->Initialize(PLAYER_POOL_SIZE)) {
			return ABNORMAL_EXIT_PLAYERPOOL_INIT_FAILED;
		}
		
		// 월드 초기화
		if (!pWorld->Initialize()) {
			return ABNORMAL_EXIT_WORLD_INIT_FAILED;
		}

		// 게임 서버 시작
		if (!pGameServer->StartServer()) {
			return ABNORMAL_EXIT_GAMESERVER_START_FAILED;
		}

		// 시스템 커맨드 입력을 받기 위한 
		pSystemWorker->Run();

		// ======================================
		WaitForSingleObject(pSystemWorker->CreateExitHandle(), INFINITE);	

		if (!pGameServer->Stop()) {
			return ABNORMAL_EXIT_GAMESERVER_STOP_FAILED;
		}

		// 플레이어 풀 정리
		if (!pPlayerPool->Finalize()) {
			return ABNORMAL_EXIT_PLAYERPOOL_FINALIZE_FAILED;
		}
		
		// 데이터베이스 정리
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
		DeleteSafe(pSystemWorker);
		DeleteSafe(pDatabase);
		DeleteSafe(pPlayerPool);
		DeleteSafe(pWorld);
		DeleteSafe(pGameServer);

		// 메모리 누수 확인
		const int iLeakMemory = detector.StopDetect();
		if (iLeakMemory == 0) {
			SafeConsole::WriteLine(ConsoleColor::LightGreen, "[축하합니다!] 메모리 누수가 없네요 우왕!!!!!!!");
			SafeConsole::WriteLine(ConsoleColor::LightGreen, "[축하합니다!] 참 잘하셨어요 ㅎㅎ (자화자찬)");
		} else {
			SafeConsole::WriteLine(ConsoleColor::LightMagenta, "[이런..] %d 바이트 만큼 누수가 있어요! 얼른 고치세요.\n", iLeakMemory);
		}
		SafeConsole::SetColor(ConsoleColor::LightGray);
	}
	return 0;
}
