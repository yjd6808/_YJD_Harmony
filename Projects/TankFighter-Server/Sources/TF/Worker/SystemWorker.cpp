/*
 * 작성자 : 윤정도
 */

#include <TF/PrecompiledHeader.h>
#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <TF/Worker/SystemWorker.h>
#include <JCore/Utils/Console.h>
#include <TF/Game/World.h>

using namespace JCore;

SystemWorker* SystemWorker::GetInstance() {
	if (ms_pInstance == nullptr) {
		ms_pInstance = new SystemWorker();
	}
	return ms_pInstance;
}

SystemWorker::SystemWorker() {}
SystemWorker::~SystemWorker() {
	if (m_hExitHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hExitHandle);
	}
};

WinHandle SystemWorker::CreateExitHandle() {
	return m_hExitHandle = CreateEventA(NULL, FALSE, FALSE, NULL);
}

void SystemWorker::Run() {
	// std::thread 멤버 함수로 실행하는 법
	// @참고 : https://stackoverflow.com/questions/10673585/start-thread-with-member-function

	m_Thread = std::thread{ [this] { WorkerThread(); } };
	m_Thread.detach();
}

void SystemWorker::WorkerThread() const {
	SafeConsole::WriteLine("SystemWorker 쓰레드가 실행되었습니다. (%d)", std::this_thread::get_id());

	for (;;) {
		char com;
		std::cout << "[서버 커맨드] 종료[c], 월드정보[i] > ";

		if (std::cin >> com) {
			if (com == 'c' || com == 'C') {
				SetEvent(m_hExitHandle);
				break;
			}

		  if (com == 'i' || com == 'I') {
				World* pWorld = World::GetInstance();
				auto channels = pWorld->GetChannels();

				SafeConsole::WriteLine(ConsoleColor::LightGray, "[통합] : 인원 : %d명 / 방의 수 : %d개 / 게임중인 방의 수 : %d",
					pWorld->GetTotalPlayerCount(), pWorld->GetTotalRoomCount(), pWorld->GetTotalPlayingRoomCount());

				for (int i = 0; i < channels.Size(); i++) {
					SafeConsole::WriteLine(ConsoleColor::Green, "[채널 %d] : 인원 : %d명 / 방의 수 : %d개 / 게임중인 방의 수 : %d", 
						channels[i]->GetChannelUID(), channels[i]->GetPlayerCount(), channels[i]->GetRoomCount(), channels[i]->GetPlayingRoomCount());
				}

				SafeConsole::SetColor(ConsoleColor::LightGray);
			}
		}
	}

	SafeConsole::WriteLine("SystemWorker 쓰레드가 종료되었습니다. (%d)", std::this_thread::get_id());
}
