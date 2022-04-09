/*
 * �ۼ��� : ������
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
				case Signal::Type::Stop:	DeleteSafe(pSignal);  goto THREAD_END;
				case Signal::Type::Packet:	ProcessPacketSignal(dynamic_cast<PacketSignal*>(pSignal)); break;
				default: DebugAssert(false, "�ñ׳� Ÿ���� �ȹٷ� �������ּ���."); break;
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

	// ������ �������� ���ȿ��� ���� ������ ���� �ֱ⸶�� ���ش�.
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
		auto pPacket = new Packet<BattleFieldPlayWaitEndSyn>();
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




	// =====================================================================================
	// Playing �ð��� ������ ��� �ѹ��� ������ �̺�Ʈ ���� �ۼ� ��
	// �� room�� �̹� Lock�� �Ǿ������Ƿ� �ٽ� Lock ���� ����
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
		// case �ٸ� �ñ׳ε� �߰��Ǹ� ó��
		}
	}

	delete unProcessedSignal;
}
