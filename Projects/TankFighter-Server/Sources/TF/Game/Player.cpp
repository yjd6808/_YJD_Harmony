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
	m_iRoomUID = INVALID_UID;
	m_bRoomHost = false;
	m_bReady = false;

	m_ePlayerState = PlayerState::Disconnected;
	m_LoggedInTime = 0;
	m_pSession = nullptr;

	Memory::Set(&m_CharacterInfo, sizeof(m_CharacterInfo), NULL);
	m_CharacterInfo.CharacterUID = INVALID_UID;
	Memory::Set(&m_TankMove, sizeof(m_TankMove), NULL);
	Memory::Set(&m_BattleInfo, sizeof(m_BattleInfo), NULL);
	m_BattleInfo.CharacterUID = INVALID_UID;
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
}

int Player::GetCharacterUID() {
	SpinLockGuard guard(m_PlayerMtx);
	return m_CharacterInfo.CharacterUID;
}

void Player::SetPlayerState(PlayerState playerState) {
	SpinLockGuard guard(m_PlayerMtx);
	m_ePlayerState = playerState;
}

PlayerState Player::GetPlayerState() {
	SpinLockGuard guard(m_PlayerMtx);
	return m_ePlayerState;
}

CharacterInfo Player::GetCharacterInfo() {
	SpinLockGuard guard(m_PlayerMtx);
	return  m_CharacterInfo;
}

void Player::UpdateCharacterInfo(const CharacterInfo& info) {
	SpinLockGuard guard(m_PlayerMtx);
	Memory::CopyUnsafe(&m_CharacterInfo, &info, sizeof(info));
}

int Player::GetRoomUID() {
	SpinLockGuard guard(m_PlayerMtx);
	return m_iRoomUID;
}

void Player::SetRoomUID(int roomUID) {
	SpinLockGuard guard(m_PlayerMtx);
	m_iRoomUID = roomUID;
}

void Player::SetRoomHost(bool isHost) {
	SpinLockGuard guard(m_PlayerMtx);
	m_bRoomHost = true;
}

void Player::SetReady(bool ready) {
	SpinLockGuard guard(m_PlayerMtx);
	m_bReady = ready;
}

bool Player::IsReady() {
	SpinLockGuard guard(m_PlayerMtx);
	return m_bReady;
}

void Player::LoadCharacterInfo(Out_ CharacterInfo& info) {
	SpinLockGuard guard(m_PlayerMtx);
	Memory::CopyUnsafe(&info, &m_CharacterInfo, sizeof(CharacterInfo));
}

void Player::LoadRoomCharacterInfo(Out_ RoomCharacterInfo& info) {
	SpinLockGuard guard(m_PlayerMtx);
	Memory::CopyUnsafe(&info, &m_CharacterInfo, sizeof(CharacterInfo));
	info.Ready = m_bReady;
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

void Player::LoadBattileInfo(BattleInfo& battleInfo) {
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

void Player::AddHitCount(int hitCount) {
	SpinLockGuard guard(m_PlayerMtx);
	m_BattleInfo.HitCount += hitCount;
}


void Player::InitializeBattleInfo() {
	SpinLockGuard guard(m_PlayerMtx);
	Memory::Set(&m_BattleInfo, sizeof(m_BattleInfo), NULL);
}

void Player::UnsafeInitializeBattleInfo() {
	Memory::Set(&m_BattleInfo, sizeof(m_BattleInfo), NULL);
}

void Player::UnsafeInitializeTankMove() {
	Memory::Set(&m_TankMove, sizeof(m_TankMove), NULL);
}

void Player::UpdateTankMove(TankMove& move) {
	SpinLockGuard guard(m_PlayerMtx);
	Memory::CopyUnsafe(&m_TankMove, &move, sizeof(TankMove));
}
