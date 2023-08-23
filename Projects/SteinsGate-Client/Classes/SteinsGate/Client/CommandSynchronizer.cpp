/*
 * 작성자: 윤정도
 * 생성일: 5/9/2023 4:46:07 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "CommandSynchronizer.h"


USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

bool CommandSynchronizer::RegistrationEnd = false;
CommandSynchronizer::CommandQueueHolder thread_local CommandSynchronizer::tlsCommandQueueHolder = registerPacketQueueAddress(64);

CommandSynchronizer::CommandQueueHolder CommandSynchronizer::registerPacketQueueAddress(int initCapacity) {
	if (RegistrationEnd) {
		return {};
	}

	CommandSynchronizer* pInst = Get();
	pInst->m_vIOCPThreadAccessCommandQueueList.PushBack({ Thread::GetThreadId(), &tlsCommandQueueHolder });
	return { initCapacity };
}

CommandSynchronizer::CommandHolder::CommandHolder(ClientConnectServerType_t listenerType, Session* sender, ICommand* copy) {
	int _;
	Sender = sender;
	ListenerType = listenerType;
	MemPool = tlsCommandQueueHolder.MemPool;
	Command = (ICommand*)tlsCommandQueueHolder.MemPool->DynamicPop(copy->CmdLen, _);
	Memory::CopyUnsafe(Command, copy, copy->CmdLen);
}

CommandSynchronizer::CommandHolder::~CommandHolder() {
	MemPool->DynamicPush(Command, Command->CmdLen);
}



CommandSynchronizer::CommandSynchronizer()
	: m_iPacketQueueCount(0)
{}
CommandSynchronizer::~CommandSynchronizer() {
	// IOCP 쓰레드가 삭제되기전 동적할당해준 패킷데이터들과 커맨드 홀더들을 해제해줘야한다.
	finalize();
}



void CommandSynchronizer::initialize() {
	filterUnusedCommandQueue();
	allocateCommandQueue();
	RegistrationEnd = true;
}

void CommandSynchronizer::enqueueCommand(ClientConnectServerType_t listenerType, Session* session, ICommand* cmd) {
	JCORE_LOCK_GUARD(*tlsCommandQueueHolder.Lock);
	auto pHolder = dbg_new CommandHolder(listenerType, session, cmd);
	tlsCommandQueueHolder.Queue->Enqueue(pHolder);
}

void CommandSynchronizer::processCommands() {
	for (int i = 0; i < m_iPacketQueueCount; ++i) {
		CommandQueueHolder* pIOCPCommandQueueHolder = m_vIOCPThreadAccessCommandQueueList[i].Value;
		CommandQueue* pQueue;
		{
			JCORE_LOCK_GUARD(*pIOCPCommandQueueHolder->Lock);
			pQueue = pIOCPCommandQueueHolder->Queue;
			pIOCPCommandQueueHolder->Queue = m_vSwapCommandQueue[i];
			m_vSwapCommandQueue[i] = pQueue;
		}

		while (!pQueue->IsEmpty()) {
			CommandHolder* pHolder = pQueue->Front();
			Core::Net->runCommand(pHolder->ListenerType, pHolder->Sender, pHolder->Command);
			pQueue->Dequeue();
			delete pHolder;
		}
	}
}


void CommandSynchronizer::filterUnusedCommandQueue() {
	// 필터완료 전까지는 IOCP쓰레드가 아닌 쓰레드도 생성될 수 있으므로. 완료전까지 생성된 쓸모없는 패킷큐는 걸러줘야함
	SGVector<Int32U> vIocpThreadIdList = Core::Net->getGroup()->GetIocp()->GetWorkThreadIdList();
	auto fnContained = [&vIocpThreadIdList](const IOCPThreadId$CommandQueuePair& pair) { return vIocpThreadIdList.Exist(pair.Key); };
	m_vIOCPThreadAccessCommandQueueList = m_vIOCPThreadAccessCommandQueueList.Extension().Filter(fnContained).ToVector();
	m_iPacketQueueCount = m_vIOCPThreadAccessCommandQueueList.Size();
}

void CommandSynchronizer::allocateCommandQueue() {
	auto fnAllocator = [this](const IOCPThreadId$CommandQueuePair& pair) {
		CommandQueueHolder* pHolder = pair.Value;
		CommandQueue* pReceiverQueue = dbg_new CommandQueue(pHolder->InitialCapacity);
		CommandQueue* pSwapQueue = dbg_new CommandQueue(pHolder->InitialCapacity);

		pHolder->Lock = dbg_new SGNormalLock;
		pHolder->MemPool = dbg_new SGIndexMemroyPool(true);
		pHolder->Queue = pReceiverQueue;
		m_vSwapCommandQueue.PushBack(pSwapQueue);
	};
	m_vIOCPThreadAccessCommandQueueList.ForEach(fnAllocator);
}

void CommandSynchronizer::finalize() {
	for (int i = 0; i < m_iPacketQueueCount; ++i) {
		CommandQueueHolder* pIOCPPacketQueueHolder = m_vIOCPThreadAccessCommandQueueList[i].Value;
		CommandQueue* pQueue;

		// 미처리 데이터 삭제
		{
			pQueue = pIOCPPacketQueueHolder->Queue;
			while (!pQueue->IsEmpty()) {
				delete pQueue->Front();
				pQueue->Dequeue();
			}
		}
		{
			pQueue = m_vSwapCommandQueue[i];
			while (!pQueue->IsEmpty()) {
				delete pQueue->Front();
				pQueue->Dequeue();
			}
		}

		JCORE_DELETE_SAFE(pIOCPPacketQueueHolder->Queue);
		JCORE_DELETE_SAFE(pIOCPPacketQueueHolder->Lock);
		JCORE_DELETE_SAFE(pIOCPPacketQueueHolder->MemPool);
		JCORE_DELETE_SAFE(m_vSwapCommandQueue[i]);
	}
	m_vIOCPThreadAccessCommandQueueList.Clear();
	m_vSwapCommandQueue.Clear();
	m_iPacketQueueCount = 0;

	CommandHolder::FreeAllObjects();
}

