/*
 * 작성자 : 윤정도
 */

#include <TF/PrecompiledHeader.h>
#include <TF/Game/Player.h>

using namespace JNetwork;
using namespace JCore;

void Player::Initialize() {
	SpinLockGuard guard(m_PlayerMtx);

	m_iAccountUID = INVALID_UID;
	m_iChanneldUID = INVALID_UID;
	m_bRoomHost = false;

	
	m_LoggedInTime = 0;
	m_pSession = nullptr;

	Memory::Set(&m_CharacterInfo, sizeof(m_CharacterInfo), NULL);
	m_CharacterInfo.CharacterUID = INVALID_UID;
	m_CharacterInfo.RoomUID = INVALID_UID;
	m_CharacterInfo.PlayerState = PlayerState::Disconnected;

	Memory::Set(&m_TankMove, sizeof(m_TankMove), NULL);
	Memory::Set(&m_BattleInfo, sizeof(m_BattleInfo), NULL);
	m_BattleInfo.CharacterUID = INVALID_UID;
	m_TankMove.CharacterUID = INVALID_UID;
}

void Player::SetAccountUID(int accountUID) {
	SpinLockGuard guard(m_PlayerMtx);
	m_iAccountUID = accountUID;
}

void Player::UpdateLoggedInTime() {
	SpinLockGuard guard(m_PlayerMtx);
	m_LoggedInTime = DateTime::Now();
}

DateTime Player::GetLoggedInTime() {
	SpinLockGuard guard(m_PlayerMtx);
	return m_LoggedInTime;
}

int Player::GetAccountUID() {
	SpinLockGuard guard(m_PlayerMtx);
	return m_iAccountUID;
}

void Player::SetChannelUID(int channelUID) {
	SpinLockGuard guard(m_PlayerMtx);
	m_iChanneldUID = channelUID;
}

int Player::GetChannelUID() {
	SpinLockGuard guard(m_PlayerMtx);
	return  m_iChanneldUID;
}

void Player::SetCharacterUID(int characterUID) {
	SpinLockGuard guard(m_PlayerMtx);
	m_CharacterInfo.CharacterUID = characterUID;
	m_TankMove.CharacterUID = characterUID;
	m_BattleInfo.CharacterUID = characterUID;
}

int Player::GetCharacterUID() {
	SpinLockGuard guard(m_PlayerMtx);
	return m_CharacterInfo.CharacterUID;
}

void Player::SetPlayerState(PlayerState playerState) {
	SpinLockGuard guard(m_PlayerMtx);
	m_CharacterInfo.PlayerState = playerState;
}

PlayerState Player::GetPlayerState() {
	SpinLockGuard guard(m_PlayerMtx);
	return m_CharacterInfo.PlayerState;
}

void Player::UpdateCharacterInfo(const CharacterInfo& info) {
	SpinLockGuard guard(m_PlayerMtx);
	m_CharacterInfo.CharacterUID = info.CharacterUID;
	m_CharacterInfo.Kill = info.Kill;
	m_CharacterInfo.Death = info.Death;
	m_CharacterInfo.Win = info.Win;
	m_CharacterInfo.Lose = info.Lose;
	m_CharacterInfo.Money = info.Money;
	strcpy_s(m_CharacterInfo.Name, NAME_LEN, info.Name);
}

int Player::GetRoomUID() {
	SpinLockGuard guard(m_PlayerMtx);
	return m_CharacterInfo.RoomUID;
}

void Player::SetRoomUID(int roomUID) {
	SpinLockGuard guard(m_PlayerMtx);
	m_CharacterInfo.RoomUID = roomUID;
}

void Player::SetRoomHost(bool isHost) {
	SpinLockGuard guard(m_PlayerMtx);
	m_bRoomHost = true;
}

void Player::SetReady(bool ready) {
	SpinLockGuard guard(m_PlayerMtx);
	m_CharacterInfo.Ready = ready;
}

bool Player::IsReady() {
	SpinLockGuard guard(m_PlayerMtx);
	return m_CharacterInfo.Ready;
}

void Player::LoadCharacterInfo(Out_ CharacterInfo& info) {
	SpinLockGuard guard(m_PlayerMtx);
	Memory::CopyUnsafe(&info, &m_CharacterInfo, sizeof(CharacterInfo));
}

void Player::LoadRoomCharacterInfo(Out_ RoomCharacterInfo& info) {
	SpinLockGuard guard(m_PlayerMtx);
	Memory::CopyUnsafe(&info, &m_CharacterInfo, sizeof(RoomCharacterInfo));
}

bool Player::SendAsync(ISendPacket* packet) {
	SpinLockGuard guard(m_PlayerMtx);

	if (m_pSession == nullptr) {
		return false;
	}

	return m_pSession->SendAsync(packet);
}

bool Player::Disconnect() {
	SpinLockGuard guard(m_PlayerMtx);

	if (m_pSession == nullptr) {
		return false;
	}

	return m_pSession->Disconnect();
}

bool Player::CheckNameEqual(const String& name) {
	SpinLockGuard guard(m_PlayerMtx);
	return name == m_CharacterInfo.Name;
}

void Player::LoadBattleInfo(BattleInfo& battleInfo) {
	SpinLockGuard guard(m_PlayerMtx);
	Memory::CopyUnsafe(&battleInfo, &m_BattleInfo, sizeof(BattleInfo));
}

void Player::AddBattleKillCount(int killCount) {
	SpinLockGuard guard(m_PlayerMtx);
	m_BattleInfo.Kill += killCount;
}

void Player::AddBattleDeathCount(int deathCount) {
	SpinLockGuard guard(m_PlayerMtx);
	m_BattleInfo.Death += deathCount;
}

void Player::AddFireCount(int fireCount) {
	SpinLockGuard guard(m_PlayerMtx);
	m_BattleInfo.FireCount += fireCount;
}


void Player::InitializeBattleInfo() {
	SpinLockGuard guard(m_PlayerMtx);
	Memory::Set(&m_BattleInfo, sizeof(m_BattleInfo), NULL);
}

void Player::LoadMoveInfo(TankMove& move) {
	SpinLockGuard guard(m_PlayerMtx);
	Memory::CopyUnsafe(&move, &m_TankMove, sizeof(TankMove));
}

bool Player::IsBattleState() {
	SpinLockGuard guard(m_PlayerMtx);
	return m_CharacterInfo.PlayerState >= PlayerState::RoomBattle;
}

void Player::InitializeRoomLobbyState(int roomUID) {
	SpinLockGuard guard(m_PlayerMtx);
	UnsafeSetRoomUID(roomUID);
	m_CharacterInfo.PlayerState = PlayerState::RoomLobby;
	UnsafeSetReady(false);
}

// 플레이가 정상적으로 배틀을 진행하게 되는 경우
void Player::InitializeRoomBattleState() {
	SpinLockGuard guard(m_PlayerMtx);
	UnsafeSetReady(false); // 이제 플레이어 준비는 배틀에 모두 진입했는지 여부를 확인하는데 사용함
	UnsafeInitializeBattleInfo();
	UnsafeInitializeTankMove();
	m_CharacterInfo.PlayerState = PlayerState::RoomBattle;
	m_CharacterInfo.IsDeath = false;
}

// 플레이어가 난입하게 된느 경우 초기화
void Player::InitializeRoomBattleIntruderState() {
	SpinLockGuard guard(m_PlayerMtx);
	UnsafeSetReady(true);	// 난입하는 경우는 바로 레디 상태
	UnsafeInitializeBattleInfo();
	UnsafeInitializeTankMove();
	m_CharacterInfo.PlayerState = PlayerState::RoomBattle;
	m_CharacterInfo.IsDeath = false;
}

int Player::SetRevivalLeftTime(int time) {
	SpinLockGuard guard(m_PlayerMtx);
	return m_iRevivalLeftTime = time;
}

int Player::GetRevivalLeftTime() {
	SpinLockGuard guard(m_PlayerMtx);
	return m_iRevivalLeftTime;
}

void Player::UnsafeInitializeBattleInfo() {
	m_BattleInfo = {};
	m_BattleInfo.CharacterUID = m_CharacterInfo.CharacterUID;
}

int Player::UnsafeGetRoomUID() const {
	return m_CharacterInfo.RoomUID;
}

void Player::UnsafeSetRoomUID(int roomUID) {
	m_CharacterInfo.RoomUID = roomUID;
}

void Player::UnsafeSetReady(bool ready) {
	m_CharacterInfo.Ready = ready;
}

bool Player::UnsafeGetReady() const {
	return m_CharacterInfo.Ready;
}

void Player::UnsafeInitializeTankMove() {
	Memory::Set(&m_TankMove, sizeof(m_TankMove), NULL);
	m_TankMove.CharacterUID = m_CharacterInfo.CharacterUID;
}

void Player::UnsafeSetPlayerState(PlayerState state) {
	m_CharacterInfo.PlayerState = state;
}

PlayerState Player::UnsafeGetPlayerState() {
	return m_CharacterInfo.PlayerState;
}

bool Player::UnsafeIsDeath() const {
	return m_CharacterInfo.IsDeath;		// 부활까지 남은시간이 0보다 크면 죽어있다고 함
}


bool Player::IsDeath() {
	SpinLockGuard guard(m_PlayerMtx);
	return m_CharacterInfo.IsDeath;		// 부활까지 남은시간이 0보다 크면 죽어있다고 함
}

void Player::SetDeath(bool isDeath) {
	SpinLockGuard guard(m_PlayerMtx);
	m_CharacterInfo.IsDeath = isDeath;
}

void Player::UpdateTankMove(TankMove& move) {
	SpinLockGuard guard(m_PlayerMtx);
	Memory::CopyUnsafe(&m_TankMove, &move, sizeof(TankMove));
}

void Player::SetLatency(TimeSpan latency) {
	SpinLockGuard guard(m_PlayerMtx);
	m_Latency = latency;
}

JCore::TimeSpan Player::GetLatency() {
	SpinLockGuard guard(m_PlayerMtx);
	return m_Latency;
}
