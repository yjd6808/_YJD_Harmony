/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 1:22:52 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/Vector.h>

#include <TF/Common/Base/IOpenCloseable.h>
#include <TF/Common/Base/IUpdatable.h>
#include <TF/Common/Base/IBroadcastable.h>

#include <TF/Common/Structure.h>

#include <TF/Server/Contents/Player.h>

class Room : public JCore::AccessibleObjectPool<Room>, public IBroadcastable, public IUpdatable, public IOpenCloseable
{
public:
	Room();

	void OnUpdate(const JCore::TimeSpan& elapsed) override;
	void BroadcastPacket(JNetwork::ISendPacket* packet, int state) override;
	void OnPopped() override;
	void OnPushed() override;

	void Close() override;
	void Open() override;
	bool IsClosed() override { return m_bClosed; }
	bool IsOpened() override { return !m_bClosed; }

	void SetName(const JCore::String& name);
	void SetLobby(ChannelLobby* lobby);
	void SetNameRaw(const char* name);
	void SetNameRaw(const JCore::String& name);
	JCore::String GetName();

	int GetPlayerCount();
	int GetPlayerCountRaw() const;

	bool IsEmpty();
	bool IsEmptyRaw();

	int Join(Player* player);
	bool Leave(Player* player);

	bool AddPlayer(Player* player);
	int AddPlayerRaw(Player* player);

	bool RemovePlayer(Player* player);
	bool RemovePlayerRaw(Player* player);

	void GetRoomInfo(JCORE_REF_OUT RoomInfo& info);
	void GetRoomMemberInfoList(JCORE_OUT RoomCharacterInfo* info);
	void GetRoomMemberInfoListRaw(JCORE_OUT RoomCharacterInfo* info);
	bool GetHostPlayer() const { return m_pLeader; }

	void BroadcastRoomMemberListInfo();
	void BroadcastRoomListInfo();

	void ChooseNewLeaderRaw();
private:
	mutable JCore::RecursiveLock m_Sync;

	ChannelLobby* m_pLobby;
	RoomState m_eState;
	JCore::String m_szName;
	JCore::AtomicBool m_bClosed;
	JCore::Vector<Player*> m_vPlayerList;
	Player* m_pLeader;
};


