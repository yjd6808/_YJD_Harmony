/*
	�ۼ��� : ������

	�� ä�κ��� �ϳ��� ������ �ְ� �����ϴ� ��Ʋ�ʵ� ��Ŀ ������
	���� ä�� Ŭ���������� �� Ŭ������ ���ؼ� ���� ������ ���ɵ����Ͽ���. (friend ����)
	
	���� �ֱ⸶�� �� �����鿡�� ��ġ ������ ���� �������� �濡�ٰ� �޽����� �����ϰ� ���� �� ����Ѵ�.
*/

#pragma once

#include <thread>
#include <TF/Game/Room.h>

#include <JCore/Container/ArrayQueue.h>
#include <JCore/Container/Vector.h>

class StopSignal;
class PacketSignal;
class Signal;
class BattleFieldWorker
{
protected:
	BattleFieldWorker(int channelUID, int delay)
		: m_iChannelUID(channelUID)
		, m_iDelay(delay)
		, m_SignalQueue(100)			// �ñ׳� ť �ʱ� �뷮
		, m_BattleFieldRoomMap(10)		// ��Ʋ�ʵ� �� �ʱ� �뷮
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
	void ProcessBattleFieldRoutine();						// PlayWait, Playing, EndWait ������ ��鿡 ���ؼ� �� ������Ʈ�� ó���ؾ��� ��ƾ�� �����ϵ��� �Ѵ�.
	void ProcessRoomPlayWaitState(Room* room);
	void ProcessRoomPlayingState(Room* room);
	void ProcessRoomEndWaitState(Room* room);
	void ProcessBattleFieldRoutineForRoom(Room* room);		// ProcessBattleFieldRoutine ���ο��� �� �濡 ���� ������ ���� �Լ�
	void CollectReadyWaitRooms();

	// ó�� �ȵ� �ñ׳ε� ����
	static void DeleteUnprocessedSignal(Signal* unProcessedSignal);
private:
	int m_iChannelUID;
	int m_iDelay;
	std::thread m_Thread;

	JCore::ArrayQueue<Signal*> m_SignalQueue;
	JCore::CriticalSectionMutex m_SignalQueueLock;

	JCore::HashMap<int, Room*> m_BattleFieldRoomMap;			// ��Ʋ�ʵ� ��ƾ�� �����ϴ� ���
	JCore::CriticalSectionMutex m_BattleFieldRoomMapLock;
	inline static BattleFieldWorker* ms_pInstance;

	friend class Channel;
};


/* ===============================================================
 *
 * �ܺ� ������� BattleField ������� ����ϱ� ���ؼ� ����� Signal Ŭ����
 * �߰������� �ʿ��� Type �߰��ϰ� ��ӹ޾Ƽ� ������ ��
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
		Stop,			// ������ ����
		Packet			// ��� ��Ʋ �������� �濡�ٰ� ����
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