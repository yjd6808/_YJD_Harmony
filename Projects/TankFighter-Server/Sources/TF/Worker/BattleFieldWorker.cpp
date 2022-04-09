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
#include <TF/ServerConfiguration.h>

#include <Common/Command.h>




using namespace JCore;
using namespace JNetwork;

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

		// 배틀진행이 모두 끝난 방들은 배틀필드 워커에서 제거해준다.
		CollectReadyWaitRooms();

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

	// =====================================================================================
	// RoomState 각각에 처리해줘야할 기능을 수행한다.
	// =====================================================================================
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


	// =====================================================================================
	// RoomState 별로 공통적으로 처리해줘야할 기능을 수행한다.
	// =====================================================================================

	// 공통적으로 일정주기마다 방 유저들에게 플레이어 위치 정보를 브로드 캐스팅해준다.
	// 배틀필드에 진입한 유저모두에 해당한다.
	if (room->UnsafeIsBattleFieldState()) {
		auto pLocationPacket = new Packet<BattileFieldTankUpdateSyn>();
		BattileFieldTankUpdateSyn* pBattileFieldTankUpdateSyn = pLocationPacket->Get<0>();
		int iIndexer = 0;
		room->UnsafeForEach([&iIndexer, pBattileFieldTankUpdateSyn](Player* p) {
			if (p->GetPlayerState() == PlayerState::RoomBattle) {
				p->LoadMoveInfo(pBattileFieldTankUpdateSyn->Move[iIndexer++]);
			}
		});
		pBattileFieldTankUpdateSyn->Count = iIndexer;
		room->UnsafeBroadcastInBattle(pLocationPacket);
	}

	// 게임이 진행중인 동안에만 전적 갱신을 일정 주기마다 해준다.
	if (room->UnsafeIsBattleFieldPlayingState()) {
		
	}


	room->m_iTimerTime -= m_iDelay;
}

void BattleFieldWorker::CollectReadyWaitRooms() {
	CriticalSectionLockGuard guard(m_BattleFieldRoomMapLock);
	m_BattleFieldRoomMap.Values().Extension().ForEach([this](Room* room) {
		if (!room->IsBattleFieldState()) {
			this->RemoveBattleFieldRoom(room);
		}
	});
}

void BattleFieldWorker::ProcessRoomPlayWaitState(Room* room) {
	
	// =====================================================================================
	// PlaytWait 상태에서 주기적으로 진행할 이벤트를 여기 작성 ㄱ
	// ※ room은 이미 Lock이 되어있으므로 다시 Lock 하지 말것
	// =====================================================================================



	// =====================================================================================
	// PlayWait 시간이 끝났을 경우 한번만 진행할 이벤트 여기 작성 ㄱ
	// ※ room은 이미 Lock이 되어있으므로 다시 Lock 하지 말것
	// =====================================================================================

	if (room->m_iTimerTime <= 0) {
		room->m_iTimerTime = BATTLE_FIELD_PLAYING_TIME;
		room->m_eRoomState = RoomState::Playing;
		auto pPacket = new Packet<BattleFieldPlayWaitEndSyn>();
		BattleFieldPlayWaitEndSyn* pBattleFieldPlayWaitEndSyn = pPacket->Get<0>();
		pBattleFieldPlayWaitEndSyn->RoomState = room->m_eRoomState;
		pBattleFieldPlayWaitEndSyn->LeftTime = BATTLE_FIELD_PLAYING_TIME;
		room->UnsafeBroadcastInBattle(pPacket);
	}
}

void BattleFieldWorker::ProcessRoomPlayingState(Room* room) {
	// =====================================================================================
	// Playing 상태에서 주기적으로 진행할 이벤트를 여기 작성 ㄱ
	// ※ room은 이미 Lock이 되어있으므로 다시 Lock 하지 말것
	// =====================================================================================




	// =====================================================================================
	// Playing 시간이 끝났을 경우 한번만 진행할 이벤트 여기 작성 ㄱ
	// ※ room은 이미 Lock이 되어있으므로 다시 Lock 하지 말것
	// =====================================================================================

	if (room->m_iTimerTime <= 0) {
		room->m_iTimerTime = BATTLE_FIELD_ENDWAIT_TIME;
		room->m_eRoomState = RoomState::EndWait;
		auto pPacket = new Packet<BattleFieldPlayingEndSyn>();
		BattleFieldPlayingEndSyn* pBattleFieldPlayingEndSyn = pPacket->Get<0>();
		pBattleFieldPlayingEndSyn->RoomState = room->m_eRoomState;
		pBattleFieldPlayingEndSyn->LeftTime = BATTLE_FIELD_ENDWAIT_TIME;
		room->UnsafeBroadcastInBattle(pPacket);
	}
}

void BattleFieldWorker::ProcessRoomEndWaitState(Room* room) {
	// =====================================================================================
	// EndWait 상태에서 주기적으로 진행할 이벤트를 여기 작성 ㄱ
	// ※ room은 이미 Lock이 되어있으므로 다시 Lock 하지 말것
	// =====================================================================================




	// =====================================================================================
	// EndWait 시간이 끝났을 경우 한번만 진행할 이벤트 여기 작성 ㄱ
	// ※ room은 이미 Lock이 되어있으므로 다시 Lock 하지 말것
	// =====================================================================================

	if (room->m_iTimerTime <= 0) {
		room->m_eRoomState = RoomState::ReadyWait;
		room->UnsafeForEach([room](Player* p) {
			p->InitializeRoomLobbyState(room->m_iRoomUID);
		});
		auto pPacket = new Packet<BattleFieldEndWaitEndSyn>();
		BattleFieldEndWaitEndSyn* pBattleFieldPlayingEndSyn = pPacket->Get<0>();
		pBattleFieldPlayingEndSyn->RoomState = room->m_eRoomState;
		room->UnsafeBroadcastInBattle(pPacket);
		this->RemoveBattleFieldRoom(room);
	}
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
