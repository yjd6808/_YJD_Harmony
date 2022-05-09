/*
 * �ۼ��� : ������
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
		DebugAssert(false, "�̹� ��Ʋ�ʵ� ��Ͽ� ���� �ֽ��ϴ�.");
	}
}

bool BattleFieldWorker::RemoveBattleFieldRoom(Room* room) {
	CriticalSectionLockGuard guard(m_BattleFieldRoomMapLock);

	if (m_BattleFieldRoomMap.Remove(room->GetRoomUID())) {
		return true;
	}

	DebugAssert(false, "��Ʋ�ʵ带 �������� ���� �ƴմϴ�.");
	return false;
}


void BattleFieldWorker::Run() {
	// std::thread ��� �Լ��� �����ϴ� ��
	// @���� : https://stackoverflow.com/questions/10673585/start-thread-with-member-function

	m_Thread = std::thread{ [this] { WorkerThread(); } };
}

void BattleFieldWorker::Join() {
	EnqueueSignal(new StopSignal);
	m_Thread.join();
}

void BattleFieldWorker::WorkerThread()  {
	Console::WriteLine("BattleFieldWorker �����尡 ����Ǿ����ϴ�. (%d)", std::this_thread::get_id());

	for (;;) {
		DateTime start = DateTime::Now();

		// ť�� ���� �ñ׳��� ��� �����Ҷ����� ���
		for (;;) {
			Signal* pSignal = this->DequeueSignal();

			if (pSignal == nullptr) {
				break;
			}

			switch (pSignal->GetType()) {
				case Signal::Type::Stop:	DeleteSafe(pSignal);										goto THREAD_END;
				case Signal::Type::Packet:	ProcessPacketSignal(dynamic_cast<PacketSignal*>(pSignal));	break;
				default: DebugAssert(false, "�ñ׳� Ÿ���� �ȹٷ� �������ּ���.");							break;
			}

			DeleteSafe(pSignal);
		}

		// ��Ʋ�ʵ忡�� ������ �������� ��鿡 ���ؼ� ���� ��ƾ�� �����ϵ��� ��
		ProcessBattleFieldRoutine();

		// ��Ʋ������ ��� ���� ����� ��Ʋ�ʵ� ��Ŀ���� �������ش�.
		CollectReadyWaitRooms();

		// �۾� ����ð��� �����̺��� �� ��쿡�� �׳� Sleep ���� �ʰ� ���������� ��
		const int iJobTime = DateTime::Now().Diff(start).GetTotalMiliSeconds();
		if (iJobTime > m_iDelay)
			continue;

		Sleep(m_iDelay - iJobTime);
	}

THREAD_END:

	// �ٸ� �ñ׳��� ����Ǳ����� Stop Signal�� ���� ��� �����ִ� �͵鿡 ���ؼ� ó��������.
	for (;;) {
		Signal* pSignal = this->DequeueSignal();

		if (pSignal == nullptr) {
			break;
		}

		DeleteUnprocessedSignal(pSignal);
	}

	Console::WriteLine("BattleFieldWorker �����尡 ����Ǿ����ϴ�. (%d)", std::this_thread::get_id());
}


void BattleFieldWorker::ProcessPacketSignal(PacketSignal* packetSignal) {
	auto pPacket =  packetSignal->GetPacket();
	CriticalSectionLockGuard guard(m_BattleFieldRoomMapLock);
	pPacket->AddRef();	// ������
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
	// RoomState ������ ó��������� ����� �����Ѵ�.
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
	// RoomState ���� ���������� ó��������� ����� �����Ѵ�.
	// =====================================================================================

	// ���������� �����ֱ⸶�� �� �����鿡�� �÷��̾� ��ġ ������ ��ε� ĳ�������ش�.
	// ��Ʋ�ʵ忡 ������ ������ο� �ش��Ѵ�.
	// �����̽ø��� �����ش�.
	if (room->UnsafeIsBattleFieldState()) {

		const int iRoomBattleStateUserCount = room->UnsafeGetRoomBattleStateUserCount();
		const auto pLocationPacket = new DynamicPacket<BattileFieldTankUpdateSyn>(
			BattileFieldTankUpdateSyn::CmdSizeOf(iRoomBattleStateUserCount)
		);
		pLocationPacket->Construct<0>(iRoomBattleStateUserCount);
		BattileFieldTankUpdateSyn* pBattileFieldTankUpdateSyn = pLocationPacket->Get<0>();
		int iIndexer = 0;
		room->UnsafeForEach([&iIndexer, pBattileFieldTankUpdateSyn](Player* p) {
			// ��Ʋ�ʵ忡 �ְ� ���� ������°� �ƴ� ���
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
	// PlaytWait ���¿��� �ֱ������� ������ �̺�Ʈ�� ���� �ۼ� ��
	// �� room�� �̹� Lock�� �Ǿ������Ƿ� �ٽ� Lock ���� ����
	// =====================================================================================



	// =====================================================================================
	// PlayWait �ð��� ������ ��� �ѹ��� ������ �̺�Ʈ ���� �ۼ� ��
	// �� room�� �̹� Lock�� �Ǿ������Ƿ� �ٽ� Lock ���� ����
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
	// Playing ���¿��� �ֱ������� ������ �̺�Ʈ�� ���� �ۼ� ��
	// �� room�� �̹� Lock�� �Ǿ������Ƿ� �ٽ� Lock ���� ����
	// =====================================================================================

	// ������ �������� ���ȿ��� ���� ������ ���� �ֱ⸶�� ���ش�.
	// ������ 4�� ���� �ѹ��� �����ش�. (4:1)
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
		// ���� �÷����߿� ���� ��� ��Ȱ ��Ȱ �ð���������Ʈ ���ش�.
		// �׸��� ��Ȱ�ð��� ���� ��� �ٽ� ������ְ� �� �����鿡�� �������ش�.
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
	// Playing �ð��� ������ ��� �ѹ��� ������ �̺�Ʈ ���� �ۼ� ��
	// �� room�� �̹� Lock�� �Ǿ������Ƿ� �ٽ� Lock ���� ����
	// =====================================================================================

	if (room->m_iTimerTime <= 0) {

		// 1. ���� ��������� �����ϰ� ��������� �� �ð��� �ش�.
		// 2. ����/���� ������ Ȯ���ϰ� �����ϰ� DB�� ����Ѵ�.
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

		// �̱����� ������� ���� ������ DB�� �ݿ��Ѵ�.
		QueryFn::AddWinCountAsync(iWinnerCharacterUID, 1);
		for (int i = 0; i < iRommMemberCount; i++) {
			if (pBattleFieldPlayingEndSyn->Info[i].CharacterUID != iWinnerCharacterUID) {
				QueryFn::AddLoseCountAsync(pBattleFieldPlayingEndSyn->Info[i].CharacterUID, 1);
			}
		}

		// �濡 ����� ���� ��� ��Ŷ�� �����ϰ� ������ ������.
		if (iRommMemberCount > 0)
			room->UnsafeBroadcastInBattle(pPacket);
		else
			pPacket->Release();
	}
}

void BattleFieldWorker::ProcessRoomEndWaitState(Room* room) {
	// =====================================================================================
	// EndWait ���¿��� �ֱ������� ������ �̺�Ʈ�� ���� �ۼ� ��
	// �� room�� �̹� Lock�� �Ǿ������Ƿ� �ٽ� Lock ���� ����
	// =====================================================================================




	// =====================================================================================
	// EndWait �ð��� ������ ��� �ѹ��� ������ �̺�Ʈ ���� �ۼ� ��
	// �� room�� �̹� Lock�� �Ǿ������Ƿ� �ٽ� Lock ���� ����
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
		// case �ٸ� �ñ׳ε� �߰��Ǹ� ó��
		}
	}

	delete unProcessedSignal;
}
