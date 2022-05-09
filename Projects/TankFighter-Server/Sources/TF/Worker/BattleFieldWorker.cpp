/*
 * 작성자 : 윤정도
 */

#include <TF/PrecompiledHeader.h>
#include <JCore/Time.h>
#include <JCore/Random.h>

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <TF/Worker/BattleFieldWorker.h>
#include <TF/Parser/SendFn.h>
#include <TF/Util/Console.h>
#include <TF/Game/World.h>
#include <TF/Parser/QueryFn.h>
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
				case Signal::Type::Stop:	DeleteSafe(pSignal);										goto THREAD_END;
				case Signal::Type::Packet:	ProcessPacketSignal(dynamic_cast<PacketSignal*>(pSignal));	break;
				default: DebugAssert(false, "시그널 타입을 똑바로 지정해주세요.");							break;
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
	// 딜레이시마다 보내준다.
	if (room->UnsafeIsBattleFieldState()) {

		const int iRoomBattleStateUserCount = room->UnsafeGetRoomBattleStateUserCount();
		const auto pLocationPacket = new DynamicPacket<BattileFieldTankUpdateSyn>(
			BattileFieldTankUpdateSyn::CmdSizeOf(iRoomBattleStateUserCount)
		);
		pLocationPacket->Construct<0>(iRoomBattleStateUserCount);
		BattileFieldTankUpdateSyn* pBattileFieldTankUpdateSyn = pLocationPacket->Get<0>();
		int iIndexer = 0;
		room->UnsafeForEach([&iIndexer, pBattileFieldTankUpdateSyn](Player* p) {
			// 배틀필드에 있고 아직 사망상태가 아닌 경우
			if (p->GetPlayerState() == PlayerState::RoomBattle && !p->IsDeath()) {
				p->LoadMoveInfo(pBattileFieldTankUpdateSyn->Move[iIndexer++]);
			}
		});
		room->UnsafeBroadcastInBattle(pLocationPacket);
	}

	


	room->m_iTimerTime -= m_iDelay;
	m_iStatisticsUpdateDelayCount += 1;
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
		const auto pPacket = new StaticPacket<BattleFieldPlayWaitEndSyn>();
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

	// 게임이 진행중인 동안에만 전적 갱신을 일정 주기마다 해준다.
	// 딜레이 4번 마다 한번씩 보내준다. (4:1)
	if (m_iStatisticsUpdateDelayCount >= BATTLE_FIELD_STATISTICS_UPDATE_DELAY) {
		const auto pBattleStatisticsPacket = new StaticPacket<BattleFieldStatisticsUpdateSyn>();
		BattleFieldStatisticsUpdateSyn* pBattleFieldStatisticsUpdateSyn = pBattleStatisticsPacket->Get<0>();
		int iRoomMemberCount = 0;
		room->UnsafeForEach([&iRoomMemberCount, pBattleFieldStatisticsUpdateSyn](Player* p) {
			if (p->GetPlayerState() == PlayerState::RoomBattle) {
				p->LoadBattleInfo(pBattleFieldStatisticsUpdateSyn->Info[iRoomMemberCount++]);
			}
		});
		pBattleFieldStatisticsUpdateSyn->Count = iRoomMemberCount;
		room->UnsafeBroadcastInBattle(pBattleStatisticsPacket);
		m_iStatisticsUpdateDelayCount = -1;
	}


	room->UnsafeForEach([this, room](Player* p) {
		// 게임 플레이중에 죽은 경우 부활 부활 시간을업데이트 해준다.
		// 그리고 부활시간이 끝난 경우 다시 살려내주고 방 유저들에게 공지해준다.
		if (p->GetPlayerState() == PlayerState::RoomBattle && p->GetRevivalLeftTime() > 0 && p->IsDeath()) {
			const int iLeftTime = p->SetRevivalLeftTime(p->GetRevivalLeftTime() - m_iDelay);
			
			if (iLeftTime <= 0) {
				p->SetDeath(false);
				const auto pRevivalPacket = new StaticPacket<BattleFieldRevivalSyn>();
				BattleFieldRevivalSyn* pBattleFieldRevivalSyn = pRevivalPacket->Get<0>();

				Random rand;
				pBattleFieldRevivalSyn->CharacterUID = p->GetCharacterUID();
				pBattleFieldRevivalSyn->RevivalMove.X = rand.GenerateInt(0 + 50, MAP_WIDTH - 50);
				pBattleFieldRevivalSyn->RevivalMove.Y = rand.GenerateInt(0 + 50, MAP_HEIGHT - 50);
				pBattleFieldRevivalSyn->RevivalMove.MoveDir = MoveDirection::None;
				pBattleFieldRevivalSyn->RevivalMove.RotationDir = RotateDirection::None;
				pBattleFieldRevivalSyn->RevivalMove.MoveSpeed = TANK_MOVE_SPEED;
				pBattleFieldRevivalSyn->RevivalMove.Rotation = 0.0f;
				pBattleFieldRevivalSyn->RevivalMove.RotationSpeed = TANK_ROTATION_SPEED;
				room->UnsafeBroadcastInBattle(pRevivalPacket);

				SendFn::BroadcastUpdateRoomUserAck(room, true);
			}
		}
	});

	// =====================================================================================
	// Playing 시간이 끝났을 경우 한번만 진행할 이벤트 여기 작성 ㄱ
	// ※ room은 이미 Lock이 되어있으므로 다시 Lock 하지 말것
	// =====================================================================================

	if (room->m_iTimerTime <= 0) {

		// 1. 최종 통계정보를 전달하고 통계정보를 볼 시간을 준다.
		// 2. 승자/패자 정보를 확인하고 전달하고 DB에 기록한다.
		room->m_iTimerTime = BATTLE_FIELD_ENDWAIT_TIME;
		room->m_eRoomState = RoomState::EndWait;
		const auto pPacket = new StaticPacket<BattleFieldPlayingEndSyn>();
		BattleFieldPlayingEndSyn* pBattleFieldPlayingEndSyn = pPacket->Get<0>();
		pBattleFieldPlayingEndSyn->RoomState = room->m_eRoomState;
		pBattleFieldPlayingEndSyn->LeftTime = BATTLE_FIELD_ENDWAIT_TIME;
		int iRommMemberCount = 0;
		room->UnsafeForEach([&iRommMemberCount, pBattleFieldPlayingEndSyn](Player* p) {
			if (p->GetPlayerState() == PlayerState::RoomBattle) {
				p->LoadBattleInfo(pBattleFieldPlayingEndSyn->Info[iRommMemberCount++]);
			}
		});
		pBattleFieldPlayingEndSyn->Count = iRommMemberCount;
		int iWinnerKillCount = pBattleFieldPlayingEndSyn->Info[0].Kill;
		int iWinnerCharacterUID = pBattleFieldPlayingEndSyn->Info[0].CharacterUID;

		for (int i = 1; i < iRommMemberCount; i++ ) {
			if (iWinnerKillCount < pBattleFieldPlayingEndSyn->Info[i].Kill) {
				iWinnerKillCount = pBattleFieldPlayingEndSyn->Info[i].Kill;
				iWinnerCharacterUID = pBattleFieldPlayingEndSyn->Info[i].CharacterUID;
			}
		}
		pBattleFieldPlayingEndSyn->WinnerCharacetrUID = iWinnerCharacterUID;

		// 이긴사람과 진사람에 대한 전적을 DB에 반영한다.
		QueryFn::AddWinCountAsync(iWinnerCharacterUID, 1);
		for (int i = 0; i < iRommMemberCount; i++) {
			if (pBattleFieldPlayingEndSyn->Info[i].CharacterUID != iWinnerCharacterUID) {
				QueryFn::AddLoseCountAsync(pBattleFieldPlayingEndSyn->Info[i].CharacterUID, 1);
			}
		}

		// 방에 사람이 있을 경우 패킷을 전달하고 없으면 버린다.
		if (iRommMemberCount > 0)
			room->UnsafeBroadcastInBattle(pPacket);
		else
			pPacket->Release();
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
		const auto pPacket = new StaticPacket<BattleFieldEndWaitEndSyn>();
		BattleFieldEndWaitEndSyn* pBattleFieldPlayingEndSyn = pPacket->Get<0>();
		pBattleFieldPlayingEndSyn->RoomState = room->m_eRoomState;
		room->UnsafeBroadcast(pPacket);
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
