/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 12:55:12 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Pool/AccessibleObjectPool.h>
#include <JCore/Primitives/String.h>

#include <TF/Common/Enum.h>

class Character;
class Channel;
class Room;
class GameSession;
class ChannelLobby;
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

	void OnLobbyJoin(ChannelLobby* lobby);
	void OnLobbyLeave();

	void OnChannelJoin(Channel* channel);
	void OnChannelLeave();

	void OnRoomJoin(Room* room);
	void OnRoomLeave();

	void SendPacket(JNetwork::ISendPacket* packet);

	JCORE_GETTER_SETTER(U, int,						AccountPrimaryKey,	m_iAccountPrimaryKey)
	JCORE_GETTER_SETTER(U, const JCore::String&,	AccountId,			m_szAccountId)
	JCORE_GETTER_SETTER(U, GameSession*,			Session,			m_pSession)
	JCORE_GETTER_SETTER(U, Channel*,				Channel,			m_pChannel)
	JCORE_GETTER_SETTER(U, Room*,					Room,				m_pRoom)
	JCORE_GETTER_SETTER(U, Character*,				Character,			m_pCharacter)
	JCORE_GETTER_SETTER(U, ChannelLobby*,			ChannelLobby,		m_pChannelLobby)
	JCORE_GETTER_SETTER(U, PlayerState,				PlayerState,		m_eState)

	int GetChannelPrimaryKey() const;

	bool LeaveChannel();
	bool LeaveLobby();
	bool LeaveRoom();

	bool Disconnect();

	JCore::String ToString();
private:
	PlayerState m_eState;
	int m_iAccountPrimaryKey;
	JCore::String m_szAccountId;
	GameSession* m_pSession;
	Channel* m_pChannel;
	Room* m_pRoom;
	Character* m_pCharacter;
	ChannelLobby* m_pChannelLobby;
};

