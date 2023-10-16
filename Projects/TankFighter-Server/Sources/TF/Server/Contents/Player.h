/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 12:55:12 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Pool/AccessibleObjectPool.h>
#include <JCore/Primitives/String.h>

class Channel;
class Room;
class GameSession;
class Player : public JCore::AccessibleObjectPool<Player>, public ISessionEventHandler, public IUpdatable
{
public:
	Player();
	~Player() override;

	void OnPopped() override;
	void OnPushed() override;

	void OnConnected() override;
	void OnDisconnected() override;

	void OnUpdate(const JCore::TimeSpan& elapsed) override;

	void SendPacket(JNetwork::ISendPacket* packet);

	JCORE_GETTER_SETTER(U, int,						AccountPrimaryKey,	m_iAccountPrimaryKey)
	JCORE_GETTER_SETTER(U, const JCore::String&,	AccountId,			m_szAccountId)
	JCORE_GETTER_SETTER(U, GameSession*,			Session,			m_pSession)
	JCORE_GETTER_SETTER(U, Channel*,				Channel,			m_pChannel)
	JCORE_GETTER_SETTER(U, Room*,					Room,				m_pRoom)

	int GetChannelPrimaryKey() const;


	JCore::String ToString();
private:
	int m_iAccountPrimaryKey;
	JCore::String m_szAccountId;
	GameSession* m_pSession;
	Channel* m_pChannel;
	Room* m_pRoom;
};


