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

USING_NS_JC;
USING_NS_JNET;

Player::Player()
	: m_iAccountPrimaryKey(Const::InvalidValue)
	, m_szAccountId(0)
	, m_pSession(nullptr)
	, m_pChannel(&Channel::Empty)
	, m_pRoom(&Room::Empty)
{}

Player::~Player() {
	
}


void Player::OnPopped() {
	m_pSession = nullptr;
	m_szAccountId = nullptr;
	m_pChannel = &Channel::Empty;
	m_pRoom = &Room::Empty;
}

void Player::OnPushed() {

}

void Player::OnConnected() {
}

void Player::OnDisconnected() {
	Channel* pChannel = m_pChannel;

	Core::World->RemovePlayer(this);

	if (pChannel)
		pChannel->RemovePlayer(this);
	// m_pRoom->RemovePlayer(this);

	Push(this);
}

void Player::OnUpdate(const JCore::TimeSpan& elapsed) {
}

void Player::SendPacket(ISendPacket* packet) {

	/*
	 * 이 방식은 멀티쓰레드에서 안전하지 않음!
	 * 나도 모르게 이렇게 작성했던데.. 주의하도록 하자.
	 *
	 * if (m_pSession)
	 *		m_pSession->SendAsync(packet);
	 */

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

String Player::ToString() {
	return StringUtil::Format("%s(PK:%d)", m_szAccountId.Source(), m_iAccountPrimaryKey);
}
