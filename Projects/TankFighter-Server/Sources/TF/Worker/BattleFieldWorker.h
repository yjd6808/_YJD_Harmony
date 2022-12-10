/*
	작성자 : 윤정도

	각 채널별로 하나씩 가지고 있고 동작하는 배틀필드 워커 쓰레드
	따라서 채널 클래스에서만 이 클래스에 대해서 직접 접근이 가능도록하였다. (friend 선언)
	
	일정 주기마다 방 유저들에게 위치 정보나 게임 진행중인 방에다가 메시지를 전달하고 싶을 때 사용한다.
*/

#pragma once

#include <JCore/Threading/Thread.h>
#include <JCore/Container/ArrayQueue.h>
#include <JCore/Container/Vector.h>

#include <TF/Game/Room.h>
#include <TF/ServerConfiguration.h>

class StopSignal;
class PacketSignal;
class Signal;
class BattleFieldWorker
{
protected:
	BattleFieldWorker(int channelUID, int delay)
		: m_iChannelUID(channelUID)
		, m_iDelay(delay)
		, m_SignalQueue(100)			// 시그널 큐 초기 용량
		, m_BattleFieldRoomMap(10)		// 배틀필드 룸 초기 용량
	{
	}

	void Run();
	void Join();
	
	void WorkerThread();

	~BattleFieldWorker();

	void EnqueueSignal(Signal* signal);
	void AddBattleFieldRoom(Room* room);
	bool RemoveBattleFieldRoom(Room* room);
protected:
	Signal* DequeueSignal();

	void ProcessPacketSignal(PacketSignal* packetSignal);
	void ProcessBattleFieldRoutine();						// PlayWait, Playing, EndWait 상태의 방들에 대해서 각 스테이트에 처리해야할 루틴을 실행하도록 한다.
	void ProcessRoomPlayWaitState(Room* room);
	void ProcessRoomPlayingState(Room* room);
	void ProcessRoomEndWaitState(Room* room);
	void ProcessBattleFieldRoutineForRoom(Room* room);		// ProcessBattleFieldRoutine 내부에서 각 방에 대해 수행할 서브 함수
	void CollectReadyWaitRooms();

	// 처리 안된 시그널들 정리
	static void DeleteUnprocessedSignal(Signal* unProcessedSignal);
private:
	int m_iChannelUID;
	int m_iDelay;							// 몇ms동안 Sleep 할지
	int m_iStatisticsUpdateDelayCount = 0;	// 딜레이 몇번당 한번씩 전송할지 - 4로 설정하고 m_iDelay 100이면 400ms당 한번씩 보냄


	JCore::Thread m_Thread;
	JCore::ArrayQueue<Signal*> m_SignalQueue;
	JCore::NormalLock m_SignalQueueLock;

	JCore::HashMap<int, Room*> m_BattleFieldRoomMap;			// 배틀필드 루틴을 진행하는 방들
	JCore::NormalLock m_BattleFieldRoomMapLock;
	inline static BattleFieldWorker* ms_pInstance;

	friend class Channel;
};


/* ===============================================================
 *
 * 외부 쓰레드와 BattleField 쓰레드와 통신하기 위해서 사용할 Signal 클래스
 * 추가적으로 필요한 Type 추가하고 상속받아서 구현할 것
 *
 * ============================================================== */


namespace JNetwork
{
	// forward declaration
	struct ISendPacket;
}

class Signal
{
public:
	enum class Type
	{
		Stop,			// 쓰레드 종료
		Packet			// 모든 배틀 진행중인 방에다가 전송
	};

	Type GetType() const { return m_eType; }
protected:
	Signal(Type type) : m_eType(type) {}
	virtual ~Signal() {}
private:
	Type m_eType;

	friend class BattleFieldWorker;
};

class StopSignal : public Signal
{
public:
	StopSignal() : Signal(Type::Stop) {}
	~StopSignal() override = default;
};

class PacketSignal : public Signal
{
public:
	PacketSignal(JNetwork::ISendPacket* packet)
		: Signal(Type::Packet)
		, m_pPacket(packet)
	{}
	~PacketSignal() override = default;

	JNetwork::ISendPacket* GetPacket() const { return m_pPacket; }
private:
	JNetwork::ISendPacket* m_pPacket;
};