/*
 * �ۼ��� : ������
 */

#include <TF/PrecompiledHeader.h>
#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <TF/Worker/SystemWorker.h>
#include <TF/Util/Console.h>
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

HANDLE SystemWorker::CreateExitHandle() {
	return m_hExitHandle = CreateEventA(NULL, FALSE, FALSE, NULL);
}

void SystemWorker::Run() {
	// std::thread ��� �Լ��� �����ϴ� ��
	// @���� : https://stackoverflow.com/questions/10673585/start-thread-with-member-function

	m_Thread = std::thread{ [this] { WorkerThread(); } };
	m_Thread.detach();
}

void SystemWorker::WorkerThread() const {
	Console::WriteLine("SystemWorker �����尡 ����Ǿ����ϴ�. (%d)", std::this_thread::get_id());

	for (;;) {
		char com;
		std::cout << "[���� Ŀ�ǵ�] ����[c], ��������[i] > ";

		if (std::cin >> com) {
			if (com == 'c' || com == 'C') {
				SetEvent(m_hExitHandle);
				break;
			} else if (com == 'i' || com == 'I') {
				World* pWorld = World::GetInstance();
				auto channels = pWorld->GetChannels();

				Console::WriteLine(ConsoleColor::LIGHTBLUE, "[����] : �ο� : %d�� / ���� �� : %d�� / �������� ���� �� : %d", 
					pWorld->GetTotalPlayerCount(), pWorld->GetTotalRoomCount(), pWorld->GetTotalPlayingRoomCount());

				for (int i = 0; i < channels.Size(); i++) {
					Console::WriteLine(ConsoleColor::GREEN, "[ä�� %d] : �ο� : %d�� / ���� �� : %d�� / �������� ���� �� : %d", 
						channels[i]->GetChannelUID(), channels[i]->GetPlayerCount(), channels[i]->GetRoomCount(), channels[i]->GetPlayingRoomCount());
				}

				Console::SetColor(ConsoleColor::LIGHTGRAY);
			}
		}
	}

THREAD_END:
	Console::WriteLine("SystemWorker �����尡 ����Ǿ����ϴ�. (%d)", std::this_thread::get_id());
}
