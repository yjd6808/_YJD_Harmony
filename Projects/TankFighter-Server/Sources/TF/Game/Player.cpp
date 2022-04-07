#define _WINSOCKAPI_

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

	m_CharacterInfo = {};
	m_TankState = {};
	m_ePlayerState = PlayerState::Disconnected;
	m_LoggedInTime = 0;
	m_pSession = nullptr;
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
