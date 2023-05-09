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
	pInst->m_vIOCPThreadAccessPacketQueueList.PushBack({ Thread::GetThreadId(), &tlsCommandQueueHolder });
	return { initCapacity };
}

CommandSynchronizer::CommandHolder::CommandHolder(Session* sender, ICommand* copy) {
	int _;
	Sender = sender;
	Cmd = copy->Cmd;
	CmdLen = copy->CmdLen;
	Data = (char*)tlsCommandQueueHolder.MemPool->DynamicPop(CmdLen, _);
	Memory::CopyUnsafe(Data, copy->GetData(), copy->GetDataLen());
}

CommandSynchronizer::CommandHolder::~CommandHolder() {
	tlsCommandQueueHolder.MemPool->DynamicPush(Data, CmdLen);
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

void CommandSynchronizer::enqueueCommand(Session* session, ICommand* cmd) {
	LOCK_GUARD(*tlsCommandQueueHolder.Lock);
	ICommand* pCmd = dbg_new CommandHolder(session, cmd);
	tlsCommandQueueHolder.Queue->Enqueue(pCmd);
}

void CommandSynchronizer::processCommands() {
	for (int i = 0; i < m_iPacketQueueCount; ++i) {
		CommandQueueHolder* pIOCPPacketQueueHolder = m_vIOCPThreadAccessPacketQueueList[i].Value;
		CommandQueue* pQueue;
		{
			LOCK_GUARD(*pIOCPPacketQueueHolder->Lock);
			pQueue = pIOCPPacketQueueHolder->Queue;
			pIOCPPacketQueueHolder->Queue = m_vSwapPacketQueue[i];
			m_vSwapPacketQueue[i] = pQueue;
		}

		while (!pQueue->IsEmpty()) {
			ICommand* pCmd = pQueue->Front();
			CoreNet_v->runCommand(pCmd);
			pQueue->Dequeue();
			delete pCmd;
		}
	}
}


void CommandSynchronizer::filterUnusedCommandQueue() {
	// 필터완료 전까지는 IOCP쓰레드가 아닌 쓰레드도 생성될 수 있으므로. 완료전까지 생성된 쓸모없는 패킷큐는 걸러줘야함
	SGVector<Int32U> vIocpThreadIdList = CoreNet_v->Group->GetIocp()->GetWorkThreadIdList();
	auto fnContained = [&vIocpThreadIdList](const IOCPThreadId$CommandQueuePair& pair) { return vIocpThreadIdList.Exist(pair.Key); };
	m_vIOCPThreadAccessPacketQueueList = m_vIOCPThreadAccessPacketQueueList.Extension().Filter(fnContained).ToVector();
	m_iPacketQueueCount = m_vIOCPThreadAccessPacketQueueList.Size();
}

void CommandSynchronizer::allocateCommandQueue() {
	auto fnAllocator = [this](const IOCPThreadId$CommandQueuePair& pair) {
		CommandQueueHolder* pHolder = pair.Value;
		CommandQueue* pReceiverQueue = dbg_new CommandQueue(pHolder->InitialCapacity);
		CommandQueue* pSwapQueue = dbg_new CommandQueue(pHolder->InitialCapacity);

		pHolder->Lock = dbg_new SGNormalLock;
		pHolder->MemPool = dbg_new SGIndexMemroyPool(true);
		pHolder->Queue = pReceiverQueue;
		m_vSwapPacketQueue.PushBack(pSwapQueue);
	};
	m_vIOCPThreadAccessPacketQueueList.ForEach(fnAllocator);
}

void CommandSynchronizer::finalize() {
	for (int i = 0; i < m_iPacketQueueCount; ++i) {
		CommandQueueHolder* pIOCPPacketQueueHolder = m_vIOCPThreadAccessPacketQueueList[i].Value;
		DeleteSafe(pIOCPPacketQueueHolder->Queue);
		DeleteSafe(pIOCPPacketQueueHolder->Lock);
		DeleteSafe(pIOCPPacketQueueHolder->MemPool);
		DeleteSafe(m_vSwapPacketQueue[i]);
	}
	m_vIOCPThreadAccessPacketQueueList.Clear();
	m_vSwapPacketQueue.Clear();
}

