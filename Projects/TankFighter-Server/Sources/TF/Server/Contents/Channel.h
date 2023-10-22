/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 1:01:20 PM
 * =====================
 *
 */


#pragma once

#include <TF/Common/Base/IOpenCloseable.h>
#include <TF/Server/Contents/Room.h>
#include <TF/Server/Contents/BattleFieldRoutine.h>

struct ChannelInfo;

class Channel : public IBroadcastable, public IUpdatable, public IOpenCloseable
{
public:
	Channel();
	Channel(int uid, JCore::String name, int maxPlayerCount, JCore::DateTime generatedTime);

	JCORE_GETTER(U, int, PrimaryKey, m_iPrimaryKey)
	JCORE_GETTER(U, const JCore::String&, Name, m_szName)
	JCORE_GETTER(U, int, MaxPlayerCount, m_iMaxPlayerCount)
	JCORE_GETTER(U, JCore::DateTime, GeneratedTime, m_dtGeneratedTime)

	JCORE_GETTER_SETTER(U, bool, Closed, m_bClosed)

	void Close() override;
	void Open() override;
	bool IsClosed() override { return m_bClosed; }
	bool IsOpened() override { return !m_bClosed; }

	void OnUpdate(const JCore::TimeSpan& elapsed) override;
	void BroadcastPacket(JNetwork::ISendPacket* packet, int state) override;

	ChannelInfo GetInfo();

	bool StartBattle(Room* room);
	void EndBattle(Room* room);

	int Join(Player* player);
	bool Leave(Player* player);

	int AddPlayer(Player* player);
	bool RemovePlayer(Player* player);

	JCore::String ToString() const;
private:
	mutable JCore::NormalLock m_PlayerListLock;
	JCore::HashSet<Player*> m_hsPlayerSet;

	BattleFieldRoutine m_BattleFieldRoutine;
	JCore::AtomicBool m_bClosed;

	int m_iPrimaryKey;
	JCore::String m_szName;
	int m_iMaxPlayerCount;
	JCore::DateTime m_dtGeneratedTime;		// DB테이블에 추가된 시날짜

	
};



