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
CommandSynchronizer::CommandQueueHolder thread_local CommandSynchronizer::tlsCommandQueueHolder =
	registerPacketQueueAddress(64);

CommandSynchronizer::CommandQueueHolder CommandSynchronizer::registerPacketQueueAddress(int _initCapacity)
{
	if (RegistrationEnd)
	{
		return {};
	}

	CommandSynchronizer* pInst = Get();
	pInst->iocpThreadAccessCommandQueueList_.PushBack({ Thread::GetThreadId(), &tlsCommandQueueHolder });
	return { _initCapacity };
}

CommandSynchronizer::CommandHolder::CommandHolder(ClientConnectServerType_t _listenerType, SGSession* _pSender,
                                                  JNetwork::ICommand* _pCopy)
{
	int unused;
	Sender = _pSender;
	ListenerType = _listenerType;
	MemPool = tlsCommandQueueHolder.MemPool;
	Int32U cmdLength = _pCopy->GetLength();
	Command = (JNetwork::ICommand*)tlsCommandQueueHolder.MemPool->DynamicPop(cmdLength, unused);
	Memory::CopyUnsafe(Command, _pCopy, cmdLength);
}

CommandSynchronizer::CommandHolder::~CommandHolder()
{
	MemPool->DynamicPush(Command, Command->GetLength());
}


CommandSynchronizer::CommandSynchronizer()
: packetQueueCount_(0)
{
}

CommandSynchronizer::~CommandSynchronizer()
{
	// IOCP 쓰레드가 삭제되기전 동적할당해준 패킷데이터들과 커맨드 홀더들을 해제해줘야한다.
	finalize();
}


void CommandSynchronizer::initialize()
{
	filterUnusedCommandQueue();
	allocateCommandQueue();
	RegistrationEnd = true;
}

void CommandSynchronizer::enqueueCommand(ClientConnectServerType_t _listenerType, SGSession* _pSession,
                                         JNetwork::ICommand* _pCmd)
{
	JCORE_LOCK_GUARD(*tlsCommandQueueHolder.Lock);
	auto pHolder = dbg_new CommandHolder(_listenerType, _pSession, _pCmd);
	tlsCommandQueueHolder.Queue->Enqueue(pHolder);
}

void CommandSynchronizer::processCommands()
{
	for (int i = 0; i < packetQueueCount_; ++i)
	{
		CommandQueueHolder* pIOCPCommandQueueHolder = iocpThreadAccessCommandQueueList_[i].value_;
		CommandQueue* pQueue;
		{
			JCORE_LOCK_GUARD(*pIOCPCommandQueueHolder->Lock);
			pQueue = pIOCPCommandQueueHolder->Queue;
			pIOCPCommandQueueHolder->Queue = swapCommandQueue_[i];
			swapCommandQueue_[i] = pQueue;
		}

		while (!pQueue->IsEmpty())
		{
			CommandHolder* pHolder = pQueue->Front();
			Core::Net->runCommand(pHolder->Sender, pHolder->Command);
			pQueue->Dequeue();
			delete pHolder;
		}
	}
}


void CommandSynchronizer::filterUnusedCommandQueue()
{
	// 필터완료 전까지는 IOCP쓰레드가 아닌 쓰레드도 생성될 수 있으므로. 완료전까지 생성된 쓸모없는 패킷큐는 걸러줘야함
	SGVector<Int32U> iocpThreadIdList = Core::Net->getGroup()->GetIocp()->GetWorkThreadIdList();
	auto fnContained = [&iocpThreadIdList](const IOCPThreadId$CommandQueuePair& pair)
	{
		return iocpThreadIdList.Exist(pair.key_);
	};
	iocpThreadAccessCommandQueueList_ = iocpThreadAccessCommandQueueList_.Extension().Filter(fnContained).ToVector();
	packetQueueCount_ = iocpThreadAccessCommandQueueList_.Size();
}

void CommandSynchronizer::allocateCommandQueue()
{
	auto fnAllocator = [this](const IOCPThreadId$CommandQueuePair& pair)
	{
		CommandQueueHolder* pHolder = pair.value_;
		CommandQueue* pReceiverQueue = dbg_new CommandQueue(pHolder->InitialCapacity);
		CommandQueue* pSwapQueue = dbg_new CommandQueue(pHolder->InitialCapacity);

		pHolder->Lock = dbg_new SGNormalLock;
		pHolder->MemPool = dbg_new SGIndexMemroyPool();
		pHolder->Queue = pReceiverQueue;
		swapCommandQueue_.PushBack(pSwapQueue);
	};
	iocpThreadAccessCommandQueueList_.ForEach(fnAllocator);
}

void CommandSynchronizer::finalize()
{
	for (int i = 0; i < packetQueueCount_; ++i)
	{
		CommandQueueHolder* pIOCPPacketQueueHolder = iocpThreadAccessCommandQueueList_[i].value_;
		CommandQueue* pQueue;

		// 미처리 데이터 삭제
		{
			pQueue = pIOCPPacketQueueHolder->Queue;
			while (!pQueue->IsEmpty())
			{
				delete pQueue->Front();
				pQueue->Dequeue();
			}
		}
		{
			pQueue = swapCommandQueue_[i];
			while (!pQueue->IsEmpty())
			{
				delete pQueue->Front();
				pQueue->Dequeue();
			}
		}

		JCORE_DELETE_SAFE(pIOCPPacketQueueHolder->Queue);
		JCORE_DELETE_SAFE(pIOCPPacketQueueHolder->Lock);
		JCORE_DELETE_SAFE(pIOCPPacketQueueHolder->MemPool);
		JCORE_DELETE_SAFE(swapCommandQueue_[i]);
	}
	iocpThreadAccessCommandQueueList_.Clear();
	swapCommandQueue_.Clear();
	packetQueueCount_ = 0;

	CommandHolder::FreeAllObjects();
}
