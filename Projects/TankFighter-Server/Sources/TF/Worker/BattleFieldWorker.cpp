/*
 * 작성자 : 윤정도
 */

#include <TF/PrecompiledHeader.h>
#include <JCore/Time.h>
#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <TF/Worker/BattleFieldWorker.h>
#include <TF/Parser/SendFn.h>
#include <TF/Util/Console.h>
#include <TF/Game/World.h>


using namespace JCore;


Signal* BattleFieldWorker::DequeueSignal() {
	CriticalSectionLockGuard guard(m_SignalQueueLock);
	if (!m_SignalQueue.IsEmpty()) {
		Signal* pSignal = m_SignalQueue.Front();
		m_SignalQueue.Dequeue();
		return pSignal;
	}

	return nullptr;
}


BattleFieldWorker::~BattleFieldWorker() = default;

void BattleFieldWorker::EnqueueSignal(Signal* signal) {
	CriticalSectionLockGuard guard(m_SignalQueueLock);
	m_SignalQueue.Enqueue(signal);
}

void BattleFieldWorker::AddBattleFieldRoom(Room* room) {
	CriticalSectionLockGuard guard(m_BattleFieldRoomMapLock);

	if (m_BattleFieldRoomMap.Insert(room->GetRoomUID(), room) == false) {
		DebugAssert(false, "이미 배틀필드 목록에 방이 있습니다.");
	}
}

bool BattleFieldWorker::RemoveBattleFieldRoom(Room* room) {
	CriticalSectionLockGuard guard(m_BattleFieldRoomMapLock);

	if (m_BattleFieldRoomMap.Remove(room->GetRoomUID())) {
		return true;
	}

	DebugAssert(false, "배틀필드를 진행중인 방이 아닙니다.");
	return false;
}


void BattleFieldWorker::Run() {
	// std::thread 멤버 함수로 실행하는 법
	// @참고 : https://stackoverflow.com/questions/10673585/start-thread-with-member-function

	m_Thread = std::thread{ [this] { WorkerThread(); } };
}

void BattleFieldWorker::Join() {
	EnqueueSignal(new StopSignal);
	m_Thread.join();
}

void BattleFieldWorker::WorkerThread()  {
	Console::WriteLine("BattleFieldWorker 쓰레드가 실행되었습니다. (%d)", std::this_thread::get_id());

	for (;;) {
		DateTime start = DateTime::Now();

		// 큐에 쌓인 시그널을 모두 진행할때까지 기달
		for (;;) {
			Signal* pSignal = this->DequeueSignal();

			if (pSignal == nullptr) {
				break;
			}

			switch (pSignal->GetType()) {
				case Signal::Type::Stop:	DeleteSafe(pSignal);  goto THREAD_END;
				case Signal::Type::Packet:	ProcessPacketSignal(dynamic_cast<PacketSignal*>(pSignal)); break;
				default: DebugAssert(false, "시그널 타입을 똑바로 지정해주세요."); break;
			}

			DeleteSafe(pSignal);
		}

		// 배틀필드에서 게임이 진행중인 방들에 대해서 평상시 루틴을 진행하도록 함
		ProcessBattleFieldRoutine();

		// 작업 진행시간이 딜레이보다 긴 경우에는 그냥 Sleep 하지 않고 지나가도록 함
		const int iJobTime = DateTime::Now().Diff(start).GetTotalMiliSeconds();
		if (iJobTime > m_iDelay)
			continue;

		Sleep(m_iDelay - iJobTime);
	}

THREAD_END:

	// 다른 시그널이 진행되기전에 Stop Signal로 나온 경우 남아있는 것들에 대해서 처리해주자.
	for (;;) {
		Signal* pSignal = this->DequeueSignal();

		if (pSignal == nullptr) {
			break;
		}

		DeleteUnprocessedSignal(pSignal);
	}

	Console::WriteLine("BattleFieldWorker 쓰레드가 종료되었습니다. (%d)", std::this_thread::get_id());
}


void BattleFieldWorker::ProcessPacketSignal(PacketSignal* packetSignal) {
	auto pPacket =  packetSignal->GetPacket();
	CriticalSectionLockGuard guard(m_BattleFieldRoomMapLock);
	pPacket->AddRef();	// 베리어
	m_BattleFieldRoomMap.Values().Extension().ForEach([pPacket](Room* room) {
		room->Broadcast(pPacket);
	});
	pPacket->Release();
}


void BattleFieldWorker::ProcessBattleFieldRoutine() {
	CriticalSectionLockGuard guard(m_BattleFieldRoomMapLock);
	m_BattleFieldRoomMap.Values().Extension().ForEach([this](Room* room) {
		this->ProcessBattleFieldRoutineForRoom(room);
	});
}




void BattleFieldWorker::ProcessBattleFieldRoutineForRoom(Room* room) {
	CriticalSectionLockGuard roomGuard(room->m_RoomLock);
	switch (room->m_eRoomState) {
	case RoomState::PlayWait: {
		ProcessRoomPlayWaitState(room);
		break;
	}
	case RoomState::Playing: {
		ProcessRoomPlayingState(room);
		break;
	}
	case RoomState::EndWait: {
		ProcessRoomEndWaitState(room);
		break;
	}
	}

	room->m_iTimerTime -= m_iDelay;
}

void BattleFieldWorker::ProcessRoomPlayWaitState(Room* room) {
}

void BattleFieldWorker::ProcessRoomPlayingState(Room* room) {
}

void BattleFieldWorker::ProcessRoomEndWaitState(Room* room) {
}

void BattleFieldWorker::ProcessEndRoutine() {

}

void BattleFieldWorker::DeleteUnprocessedSignal(Signal* unProcessedSignal) {
	switch (unProcessedSignal->GetType()) {
		case Signal::Type::Packet: {
			const PacketSignal* pPacketSignal = dynamic_cast<PacketSignal*>(unProcessedSignal);
			pPacketSignal->GetPacket()->Release();
		// case 다른 시그널들 추가되면 처리
		}
	}

	delete unProcessedSignal;
}
