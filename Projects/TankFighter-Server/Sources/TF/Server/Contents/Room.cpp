/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 1:23:01 PM
 * =====================
 *
 */


#include "Pch.h"
#include "Room.h"

#include <TF/Common/Command.h>
#include <TF/Server/Const.h>
#include <TF/Server/Contents/Character.h>
#include <TF/Server/Send/S_GAME.h>

#include "Query/Q_GAME.h"

USING_NS_JC;
USING_NS_JNET;

Room::Room()
	: m_pChannel(nullptr)
	, m_pLobby(nullptr)
	, m_eState(RoomState::ReadyWait)			
	, m_szName(0)
	, m_vPlayerList(0)						// Lazy-Intialization
	, m_pLeader(nullptr)
	, m_ElapsedBattleTime(TimeCounterAttribute::TimeOverReset)
{}

void Room::OnUpdate(const TimeSpan& elapsed) {
}

// 배틀필드를 진행중일 때 수행하는 업데이트
void Room::OnUpdateBattle(const TimeSpan& elapsed) {
	JCORE_LOCK_GUARD(m_Sync);

	const RoomState eCurState = m_eState;
	switch (eCurState) {
	case RoomState::PlayWait: OnUpdatePlayWait(elapsed);
	case RoomState::Playing:  OnUpdatePlaying(elapsed);	
	case RoomState::EndWait:  OnUpdateEndWait(elapsed);	
	default: ;
	}

	const RoomState eNextState = m_eState;
	if (eNextState != eCurState) {
		OnRoomStateChanged(eNextState);
	}

	m_ElapsedBattleTime.Elapsed += elapsed;
}

void Room::OnUpdatePlayWait(const TimeSpan& elapsed) {
	if (m_eState != RoomState::PlayWait)
		return;


	if (!m_ElapsedBattleTime.ElapsedSeconds(Const::BattleField::PlayWaitTime)) {
		return;
	}


	m_eState = RoomState::Playing;
}

void Room::OnUpdatePlaying(const TimeSpan& elapsed) {
	if (m_eState != RoomState::Playing)
		return;

	if (!m_ElapsedBattleTime.ElapsedSeconds(Const::BattleField::PlayingTime)) {
		return;
	}

	// 승자/패자 판단 및 DB 반영
	JudgeWinnerLoserRaw();
	m_eState = RoomState::EndWait;
}

void Room::OnUpdateEndWait(const TimeSpan& elapsed) {
	if (m_eState != RoomState::EndWait)
		return;

	if (!m_ElapsedBattleTime.ElapsedSeconds(Const::BattleField::EndWaitTime)) {
		return;
	}

	m_eState = RoomState::ReadyWait;
	m_pChannel->EndBattle(this);
}

void Room::OnRoomStateChanged(RoomState changedState) {
	BroadcastTimeSync();
	BroadcastBattleStateChanged(changedState);
}


void Room::OnPopped() {
	m_bClosed = false;
	m_eState = RoomState::ReadyWait;
}

void Room::OnPushed() {
	m_pLobby = nullptr;
	m_pChannel = nullptr;
	m_bClosed = true;
	m_pLeader = nullptr;
	m_vPlayerList.Clear();
}

void Room::OnBattleBegin() {
	m_eState = RoomState::PlayWait;

	JCORE_LOCK_GUARD(m_Sync);
	for (int i = 0; i < m_vPlayerList.Size(); ++i) {
		m_vPlayerList[i]->OnBattleBegin();
	}

	BroadcastRoomGameStart();
}

void Room::OnBattleEnd() {
	m_eState = RoomState::ReadyWait;

	{
		JCORE_LOCK_GUARD(m_Sync);
		for (int i = 0; i < m_vPlayerList.Size(); ++i) {
			m_vPlayerList[i]->OnBattleEnd();
		}

	}
	BroadcastRoomGameEnd();
}


void Room::JudgeWinnerLoserRaw() {
	Character* pWinner = GetBattleTopKillerRaw();
	if (pWinner != nullptr) {
		pWinner->AddWinCount(1);
		Q_GAME::AddWinCount(pWinner->GetPrimaryKey(), 1);
	}

	for (int i = 0; i < m_vPlayerList.Size(); ++i) {
		Character* pLoser = m_vPlayerList[i]->GetCharacter();
		if (pLoser == pWinner) continue;	// 승자 제외
		pLoser->AddLoseCount(1);
		Q_GAME::AddWinCount(pLoser->GetPrimaryKey(), 1);
	}

	BroadcastJudgeRaw(pWinner == nullptr ? Const::InvalidValue : pWinner->GetPrimaryKey());
}


void Room::BroadcastPacket(IPacket* packet, int state) {
	auto fnStateAny = [=](Player* player) {
		player->SendPacket(packet);
	};

	auto fnStateBattle = [=](Player* player) {
		if (player->GetPlayerState() == PlayerState::BattleField)
			player->SendPacket(packet);
	};

	if (state == Const::Broadcast::Room::StateAny) {
		m_vPlayerList.ForEach(fnStateAny);
	} else if (state == Const::Broadcast::Room::StateBattle) {
		m_vPlayerList.ForEach(fnStateBattle);
	}
}

void Room::Close() {
}

void Room::Open() {
}

bool Room::IsAllReady() const {
	int iReadyCount = 0;
	JCORE_LOCK_GUARD(m_Sync);
	for (int i = 0; i < m_vPlayerList.Size(); ++i) {
		const Character* pCharacter = m_vPlayerList[i]->GetCharacter();
		if (pCharacter == nullptr) continue;
		if (pCharacter->IsReady()) {
			++iReadyCount;
		}
	}
	return iReadyCount == m_vPlayerList.Size();
}

void Room::SetName(const String& name) {
	JCORE_LOCK_GUARD(m_Sync);
	m_szName = name;
}

void Room::SetLobby(ChannelLobby* lobby) {
	m_pLobby = lobby;
}

void Room::SetChannel(Channel* channel) {
	m_pChannel = channel;
}

void Room::SetNameRaw(const char* name) {
	m_szName = name;
}

void Room::SetState(RoomState state) {
	m_eState = state;
}

void Room::SetNameRaw(const String& name) {
	m_szName = name;
}

String Room::GetName() {
	String szName = String::Null;
	{
		JCORE_LOCK_GUARD(m_Sync);
		szName = m_szName;
	}
	return szName;
}

int Room::GetPlayerCount() {
	JCORE_LOCK_GUARD(m_Sync);
	return m_vPlayerList.Size();
}

bool Room::IsEmpty() {
	JCORE_LOCK_GUARD(m_Sync);
	return m_vPlayerList.Size() <= 0;
}

int Room::Join(Player* player) {
	{
		JCORE_LOCK_GUARD(m_Sync);

		if (m_bClosed) {
			return Const::Failed::Room::AddFailedClosed;
		}

		const int iResult = AddPlayerRaw(player);
		if (iResult != Const::Success)
			return iResult;

		if (m_vPlayerList.Size() == 1) {
			ChooseNewLeaderRaw();
		}
	}


	player->OnRoomJoin(this);
	m_pLobby->BroadcastRoomListInfo();
	BroadcastRoomMemberListInfo();
	BroadcastRoomInfo();
	return Const::Success;
}

bool Room::Leave(Player* player) {
	JCORE_LOCK_GUARD(m_Sync);

	const bool bPlayerRemoved = RemovePlayerRaw(player);
	const bool bHost = m_pLeader == player;
	const bool bEmpty = IsEmptyRaw();

	if (bEmpty) {
		m_pLobby->RemoveRoom(this);
		m_pChannel->EndBattle(this);
		Push(this);
	} else {
		if (bHost && bPlayerRemoved)
			ChooseNewLeaderRaw();
	}

	if (bPlayerRemoved) {
		m_Sync.Unlock();
		player->OnRoomLeave();

		m_pLobby->BroadcastRoomListInfo();
		BroadcastRoomMemberListInfo();
		BroadcastRoomInfo();
		BroadcastBattleFieldLeave(player);
		BroadcastBattleSatistics();

		m_Sync.Lock();
	}

	
	return bPlayerRemoved;
}

bool Room::AddPlayer(Player* player) {
	JCORE_LOCK_GUARD(m_Sync);
	return AddPlayerRaw(player);
}

bool Room::RemovePlayer(Player* player) {
	JCORE_LOCK_GUARD(m_Sync);
	return RemovePlayerRaw(player);
}

void Room::GetRoomInfo(JCORE_REF_OUT RoomInfo& info) {
	info.AccessId = m_iAccessId;
	info.MaxPlayerCount = Const::Room::MaxPlayerCount;
	info.RoomState = m_eState;

	{
		JCORE_LOCK_GUARD(m_Sync);
		info.PlayerCount = m_vPlayerList.Size();
		info.Name = m_szName;
	}
}

void Room::GetRoomMemberInfoList(JCORE_OUT RoomCharacterInfo* info) {
	JCORE_LOCK_GUARD(m_Sync);
	GetRoomMemberInfoListRaw(info);
}

Vector<TankMoveNet> Room::GetRoomMemberLiveMoveList() {
	Vector<TankMoveNet> vMoves{ Const::Room::MaxPlayerCount };
	JCORE_LOCK_GUARD(m_Sync);
	for (int i = 0; i < m_vPlayerList.Size(); ++i) {

		if (m_vPlayerList[i]->GetPlayerState() != PlayerState::BattleField) {
			continue;
		}

		Character* pCharacter = m_vPlayerList[i]->GetCharacter();

		if (pCharacter == nullptr) {
			continue;
		}

		if (pCharacter->IsDeath()) {
			continue;
		}

		TankMoveNet moveNet;
		pCharacter->GetMoveNet(moveNet);
		vMoves.PushBack(moveNet);
	}
	return vMoves;
}

void Room::GetRoomMemberInfoListRaw(JCORE_OUT RoomCharacterInfo* info) {
	for (int i = 0; i < m_vPlayerList.Size(); ++i) {
		Character* pCharacter = m_vPlayerList[i]->GetCharacter();

		if (pCharacter == nullptr) {
			info[i] = {};
			continue;
		}

		pCharacter->GetRoomInfo(info[i]);
	}
}

void Room::BroadcastRoomMemberListInfo() {
	Player* pLeader = m_pLeader;

	if (pLeader == nullptr) {
		DebugAssert(false);
		return;
	}

	Character* pLeaderCharacter = pLeader->GetCharacter();

	if (pLeaderCharacter == nullptr) {
		DebugAssert(false);
		return;
	}

	JCORE_LOCK_GUARD(m_Sync);
	int iCount = m_vPlayerList.Size();
	auto pPacket = dbg_new SinglePacket<SC_UpdateRoomMemberList>(iCount);
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	pPacket->Cmd.HostCharacterPrimaryKey = pLeaderCharacter->GetPrimaryKey();
	GetRoomMemberInfoListRaw(pPacket->Cmd.Info);
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateAny);
}

void Room::BroadcastRoomMemberInfo(Character* character) {
	auto pPacket = dbg_new SinglePacket<SC_UpdateRoomMember>();
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	character->GetRoomInfo(pPacket->Cmd.Info);
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateAny);
}

void Room::BroadcastRoomInfo() {
	auto pPacket = dbg_new SinglePacket<SC_LoadRoomInfo>();
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	GetRoomInfo(pPacket->Cmd.Info);
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateAny);
}

void Room::BroadcastRoomGameStart() {
	auto pPacket = dbg_new SinglePacket<SC_RoomGameStart>;
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateAny);
}

void Room::BroadcastBattleFieldTankSpawn(Character* character, bool revival) {
	auto pPacket = dbg_new SinglePacket<SC_BattleFieldTankSpawn>();
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	character->Spawn();
	character->GetMoveNet(pPacket->Cmd.Move);
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateAny);
}

void Room::BroadcastBattleSatistics() {
	JCORE_LOCK_GUARD(m_Sync);
	const auto pPacket = dbg_new SinglePacket<SC_BattleFieldStatisticsUpdate>(GetBattleFieldPlayerCountRaw());
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	for (int i = 0; i < m_vPlayerList.Size(); ++i) {
		Player* pPlayer = m_vPlayerList[i];
		if (pPlayer->GetPlayerState() != PlayerState::BattleField)
			continue;

		BattleStatisticsNet& dst = pPacket->Cmd.Statistics[i];
		pPlayer->GetCharacter()->GetBattleStatisticsNet(dst);
	}
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateBattle);
}

void Room::BroadcastBattleFieldFire(const BulletInfoNet& info) {
	const auto pPacket = dbg_new SinglePacket<SC_BattleFieldFire>();
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	pPacket->Cmd.BulletInfo = info;
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateBattle);
}

void Room::BroadcastBattleStateChanged(RoomState changedState) {
	const auto vRoomInfoList = m_pLobby->GetRoomInfoList();
	const auto pPacket = dbg_new SinglePacket<SC_BattleFieldStateChanged>(vRoomInfoList.Size());
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	pPacket->Cmd.State = changedState;
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateBattle);
}

void Room::BroadcastBattleFieldLeave(Player* outPlayer) {
	if (outPlayer->GetPlayerState() != PlayerState::BattleField) {
		return;
	}

	Character* pCharacter = outPlayer->GetCharacter();
	const auto pPacket = dbg_new SinglePacket<SC_BattleFieldLeave>();
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	pPacket->Cmd.CharacterPrimaryKey = pCharacter->GetPrimaryKey();
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateBattle);
}

void Room::BroadcastJudgeRaw(int winnerCharacterPrimaryKey) {
	auto pPacket = dbg_new SinglePacket<SC_RoomGameJudge>;
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	pPacket->Cmd.WinnerCharacterPrimaryKey = winnerCharacterPrimaryKey;
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateBattle);
}

void Room::BroadcastRoomGameEnd() {
	auto pPacket = dbg_new SinglePacket<SC_RoomGameEnd>;
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateAny);
}

void Room::BroadcastTimeSync() {
	const auto pPacket = dbg_new SinglePacket<SC_BattleFieldTimeSync>();
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	pPacket->Cmd.Elapsed.Tick = m_ElapsedBattleTime.Elapsed.Tick;
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateBattle);
}

void Room::BroadcastBattleFieldMove(const TankMoveNet& move) {
	const auto pPacket = dbg_new SinglePacket<SC_BattleFieldMove>();
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	pPacket->Cmd.Move = move;
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateBattle);
}

void Room::BroadcastBattleFieldDeath(int deadCharacterPrimaryKey) {
	const int iCount = m_vPlayerList.Size();
	const auto pPacket = dbg_new SinglePacket<SC_BattleFieldDeath>(iCount);
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	pPacket->Cmd.DeadCharacterPrimaryKey = deadCharacterPrimaryKey;
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateBattle);
}

void Room::ChooseNewLeaderRaw() {
	const int iLeftPlayerCount = m_vPlayerList.Size();
	if (iLeftPlayerCount <= 0) {
		return;
	}

	m_pLeader = m_vPlayerList[0];
	m_pLeader->GetCharacter()->SetReady(true);	// 방장은 기본적으로 레디 상태
}

int Room::AddPlayerRaw(Player* player) {
	if (m_vPlayerList.Size() >= Const::Room::MaxPlayerCount) {
		return Const::Failed::Room::AddFailedFull;
	}

	for (int i = 0; i < m_vPlayerList.Size(); ++i) {
		if (m_vPlayerList[i] == player)
			return Const::Failed::Room::AddFailedAlreadyExist;
	}

	m_vPlayerList.PushBack(player);
	return Const::Success;
}

bool Room::RemovePlayerRaw(Player* player) {
	return m_vPlayerList.Remove(player);
}

int Room::GetPlayerCountRaw() const {
	return m_vPlayerList.Size();
}

int Room::GetBattleFieldPlayerCountRaw() const {
	int iCount = 0;
	for (int i = 0; i < m_vPlayerList.Size(); ++i)
		if (m_vPlayerList[i]->GetPlayerState() == PlayerState::BattleField)
			iCount++;
	return iCount;
}

Character* Room::GetBattleTopKillerRaw() const {
	const int iCount = m_vPlayerList.Size();

	Character* pTopKiller = nullptr;
	int iTopKillCount = -1;				// 최고 킬 수
	DateTime dtTopLastKillTime = 0;		// 이 킬을 가장빨리 달성한 시각

	for (int i = 0; i < iCount; ++i) {
		Character* pCharacter = m_vPlayerList[i]->GetCharacter();
		if (pCharacter == nullptr)
			continue;

		const int iBattleKillCount = pCharacter->GetBattleKillCount();
		DateTime dtLastKillTime = pCharacter->GetBattleLastKillTime();

		if (iBattleKillCount == iTopKillCount && dtLastKillTime < dtTopLastKillTime) {
			pTopKiller = pCharacter;
			dtTopLastKillTime = dtLastKillTime;
			continue;
		}

		if (iBattleKillCount > iTopKillCount) {
			pTopKiller = pCharacter;
			iTopKillCount = iBattleKillCount;
			dtTopLastKillTime = dtLastKillTime;
		}
	}

	// 아무도 킬을 안한 경우는 톱브킬러 없다고 처리
	if (iTopKillCount == 0) {
		return nullptr;
	}

	return pTopKiller;
}

Character* Room::GetCharacterByCharacterPrimaryKey(int characterPrimaryKey) const {
	JCORE_LOCK_GUARD(m_Sync);
	const int iCount = m_vPlayerList.Size();
	for (int i = 0; i < iCount; ++i) {
		Character* pCharacter = m_vPlayerList[i]->GetCharacter();
		if (pCharacter && pCharacter->GetPrimaryKey() == characterPrimaryKey) {
			return pCharacter;
		}
	}
	return nullptr;
}

bool Room::IsEmptyRaw() {
	return m_vPlayerList.Size() == 0;
}
