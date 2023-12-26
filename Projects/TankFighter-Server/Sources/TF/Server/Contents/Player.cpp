/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 12:55:24 PM
 * =====================
 *
 */

#include "Pch.h"
#include "Player.h"

#include <TF/Server/Const.h>
#include <TF/Server/Host/GameSession.h>
#include <TF/Server/Contents/Character.h>
#include <TF/Server/Send/S_GAME.h>

USING_NS_JC;
USING_NS_JNET;

// Empty로 둔 이유
// null체크 하기 귀찮아서 그냥 null인 상태를 안만들도록 함.
// 이렇게 해보고 불편한점이 생기면 슈타인즈 게이트 프로젝트에선 이렇게 하지말자.
Player::Player()
	: m_eState(PlayerState::Initialized)
	, m_iAccountPrimaryKey(Const::InvalidValue)
	, m_szAccountId(0)
	, m_pSession(nullptr)
	, m_pChannel(nullptr)
	, m_pRoom(nullptr)
	, m_pCharacter(nullptr)
	, m_pChannelLobby(nullptr)
{}

Player::~Player() {
	
}


void Player::OnPopped() {

}

void Player::OnPushed() {
	m_pSession = nullptr;
	m_szAccountId = nullptr;
	m_pChannel = nullptr;
	m_pRoom = nullptr;
	m_pCharacter = nullptr;
	m_pChannelLobby = nullptr;
}

void Player::OnConnected() {
}

void Player::OnDisconnected() {
	m_pSession->SetPlayer(nullptr);
	Core::World->RemovePlayer(this);
	LeaveChannel();
	Push(this);
}

void Player::OnUpdate(const TimeSpan& elapsed) {
	Character* pCharacter = m_pCharacter;

	if (pCharacter)
		pCharacter->OnUpdate(elapsed);
}

void Player::OnLobbyJoin(ChannelLobby* lobby) {
	m_pChannelLobby = lobby;
	m_pCharacter->NotifyLoginStateToFriends(true);
}

void Player::OnLobbyLeave() {
	LeaveRoom();

	Character* pCharacter = m_pCharacter;

	if (pCharacter != nullptr) {
		pCharacter->NotifyLoginStateToFriends(false);
		Character::Push(pCharacter);
	}

	m_pChannelLobby = nullptr;
	m_pCharacter = nullptr;
}

void Player::OnChannelJoin(Channel* channel) {
	m_pChannel = channel;
}

void Player::OnChannelLeave() {
	LeaveLobby();

	m_pChannel = nullptr;
}

void Player::OnRoomJoin(Room* room) {
	m_pRoom = room;
	m_pCharacter->SetRoom(room);
}

void Player::OnRoomLeave() {
	m_pRoom = nullptr;
	m_pCharacter->SetRoom(nullptr);
}

void Player::OnBattleBegin() {
	m_eState = PlayerState::BattleField;
	Character* pCharacter = m_pCharacter;
	if (pCharacter == nullptr)
		return;
	pCharacter->ClearBattleInfo();
	pCharacter->SetRevivalTime(0);
}

void Player::OnBattleEnd() {
	m_eState = PlayerState::Room;

	Character* pCharacter = m_pCharacter;
	if (pCharacter == nullptr)
		return;

	pCharacter->ApplyBattleStatisticsToInfo();
	pCharacter->ClearBattleInfo();
}

void Player::SendPacket(IPacket* packet) {
	Session* session = m_pSession;

	if (session == nullptr) 
		return;

	session->SendAsync(packet);
}

int Player::GetChannelPrimaryKey() const {
	Channel* pChannel = m_pChannel;

	if (pChannel == nullptr)
		return Const::InvalidValue;

	return pChannel->GetPrimaryKey();
}

bool Player::LeaveChannel() {
	Channel* pChannel = m_pChannel;

	if (pChannel == nullptr)
		return false;

	return pChannel->Leave(this);
}

bool Player::LeaveLobby() {
	ChannelLobby* pChannelLobby = m_pChannelLobby;

	if (pChannelLobby == nullptr)
		return false;

	return pChannelLobby->Leave(this);
}

bool Player::LeaveRoom() {
	Room* pRoom = m_pRoom;
	if (pRoom == nullptr)
		return false;

	return pRoom->Leave(this);
}

bool Player::Disconnect() {
	Session* session = m_pSession;

	if (session == nullptr)
		return false;

	return session->Disconnect();
}

String Player::ToString() {
	return StringUtil::Format("%s(PK:%d)", m_szAccountId.Source(), m_iAccountPrimaryKey);
}
