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
	void SetNameRaw(const char* name);
	void SetNameRaw(const JCore::String& name);
	JCore::String GetName();

	int GetPlayerCount();
	bool IsEmpty();

	int Join(Player* player);
	bool Leave(Player* player);

	bool AddPlayer(Player* player);
	bool RemovePlayer(Player* player);

	RoomInfo GetRoomInfo();
	bool GetHostPlayer() const { return m_pLeader; }
private:
	void ChooseNewLeaderRaw();
	int AddPlayerRaw(Player* player);
	bool RemovePlayerRaw(Player* player);
	int GetPlayerCountRaw() const;
	bool IsEmptyRaw();

	RoomState m_eState;
	JCore::String m_szName;
	JCore::AtomicBool m_bClosed;
	JCore::NormalLock m_Sync;
	JCore::Vector<Player*> m_vPlayerList;
	Player* m_pLeader;
};


