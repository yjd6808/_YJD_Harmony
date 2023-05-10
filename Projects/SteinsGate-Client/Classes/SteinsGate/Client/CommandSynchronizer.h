/*
 * 작성자: 윤정도
 * 생성일: 5/9/2023 3:29:07 AM
 * =====================
 * 여러 IOCP 쓰레드에서 수신한 커맨드들을 코코스 쓰레드로 받아서 처리하기 위함
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

using CommandQueue = SGArrayQueue<JNetwork::ICommand*>;
class CommandSynchronizer final : public SGSingletonPointer<CommandSynchronizer>
{
	struct CommandQueueHolder
	{
		CommandQueueHolder()
			: InitialCapacity(0)
			, Queue(nullptr)
			, MemPool(nullptr)
			, Lock(nullptr)
		{}
		CommandQueueHolder(int capacity)
			: InitialCapacity(capacity)
			, Queue(nullptr)
			, MemPool(nullptr)
			, Lock(nullptr)
		{}
			
		int InitialCapacity;
		CommandQueue* Queue;
		SGIndexMemroyPool* MemPool;	// 데이터를 반환해줄 메모리풀
		SGNormalLock* Lock;
	};

	struct CommandHolder : JNetwork::ICommand, SGObjectPool<CommandHolder>
	{
		CommandHolder()
			: MemPool(nullptr)
			, ListenerType(ClientConnectServerType::Max)
			, Sender(nullptr)
			, Data(nullptr)
		{}
		CommandHolder(ClientConnectServerType_t listenerType, SGSession* sender, ICommand* copy);
		~CommandHolder() override;

		SGIndexMemroyPool* MemPool;	// 데이터를 돌려놓을 메모리풀(홀더 해제를 메인쓰레드에서 수행하기 때문에 포인터정보가 필요함)
		ClientConnectServerType_t ListenerType;
		SGSession* Sender;
		char* Data;
	};

	using IOCPThreadId$CommandQueuePair = JCore::Pair<Int32U, CommandQueueHolder*>;			// IOCP 쓰레드의 ID와 커맨드큐 페어
	using IOCPThreadId$CommandQueuePairList = SGVector<IOCPThreadId$CommandQueuePair>;

	friend class TSingleton;
	CommandSynchronizer();
	~CommandSynchronizer() override;

	void filterUnusedCommandQueue();	// 안쓰는 패킷큐는 해제 (메인 쓰레드에서 생성된 큐의 경우 쓸일이 없으므로)
	void allocateCommandQueue();
	void finalize();
	static CommandQueueHolder registerPacketQueueAddress(int initCapacity);
public:
	void enqueueCommand(ClientConnectServerType_t listenerType, SGSession* sender, JNetwork::ICommand* cmd);
	void initialize();
	void processCommands();
private:
	int m_iPacketQueueCount;
	SGVector<CommandQueue*> m_vSwapCommandQueue;
	IOCPThreadId$CommandQueuePairList m_vIOCPThreadAccessCommandQueueList;

	static bool RegistrationEnd;
	static thread_local CommandQueueHolder tlsCommandQueueHolder;
};


