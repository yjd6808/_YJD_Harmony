/*
 * 작성자: 윤정도
 * 생성일: 5/9/2023 4:46:07 AM
 * =====================
 *
 */


#include "Core.h"
#include "_Net/CommandSynchronizer.h"

#include <sgcl/NetCore.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

bool CommandSynchronizer::RegistrationEnd = false;
CommandSynchronizer::CommandQueueHolder thread_local CommandSynchronizer::tlsCommandQueueHolder = RegisterPacketQueueAddress(64);

CommandSynchronizer::CommandQueueHolder CommandSynchronizer::RegisterPacketQueueAddress(int _initCapacity)
{
	if (RegistrationEnd)
	{
		return {};
	}

	CommandSynchronizer* pInst = Get();
	pInst->iocpThreadAccessCommandQueueList_.PushBack({ Thread::GetThreadId(), &tlsCommandQueueHolder });
	return { _initCapacity };
}

//////////////////////////////////////////////////////////////////////////////////////////
CommandSynchronizer::CommandHolder::CommandHolder(ClientConnectServerType_t _listenerType, jnet::Session* _pSender, jnet::ICommand* _pCopy)
{
	int unused;
	pSender_ = _pSender;
	listenerType_ = _listenerType;
	pMemPool_ = tlsCommandQueueHolder.pMemPool_;
	Int32U cmdLength = _pCopy->GetLength();
	pCommand_ = (jnet::ICommand*)tlsCommandQueueHolder.pMemPool_->DynamicPop(cmdLength, unused);
	Memory::CopyUnsafe(pCommand_, _pCopy, cmdLength);
}

//////////////////////////////////////////////////////////////////////////////////////////
CommandSynchronizer::CommandHolder::~CommandHolder()
{
	pMemPool_->DynamicPush(pCommand_, pCommand_->GetLength());
}

//////////////////////////////////////////////////////////////////////////////////////////
CommandSynchronizer::CommandSynchronizer()
: packetQueueCount_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
CommandSynchronizer::~CommandSynchronizer()
{
	// IOCP 쓰레드가 삭제되기전 동적할당해준 패킷데이터들과 커맨드 홀더들을 해제해줘야한다.
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommandSynchronizer::Initialize()
{
	FilterUnusedCommandQueue();
	AllocateCommandQueue();
	RegistrationEnd = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommandSynchronizer::EnqueueCommand(ClientConnectServerType_t _listenerType, jnet::Session* _pSession,
                                         jnet::ICommand* _pCmd)
{
	JC_LOCK_GUARD(*tlsCommandQueueHolder.pLock_);
	auto pHolder = dbg_new CommandHolder(_listenerType, _pSession, _pCmd);
	tlsCommandQueueHolder.pQueue_->Enqueue(pHolder);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommandSynchronizer::ProcessCommands()
{
	for (int i = 0; i < packetQueueCount_; ++i)
	{
		CommandQueueHolder* pIOCPCommandQueueHolder = iocpThreadAccessCommandQueueList_[i].value_;
		CommandQueue* pQueue;
		{
			JC_LOCK_GUARD(*pIOCPCommandQueueHolder->pLock_);
			pQueue = pIOCPCommandQueueHolder->pQueue_;
			pIOCPCommandQueueHolder->pQueue_ = swapCommandQueue_[i];
			swapCommandQueue_[i] = pQueue;
		}

		while (!pQueue->IsEmpty())
		{
			CommandHolder* pHolder = pQueue->Front();
			g_cNet.RunCommand(pHolder->pSender_, pHolder->pCommand_);
			pQueue->Dequeue();
			delete pHolder;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommandSynchronizer::FilterUnusedCommandQueue()
{
	// 필터완료 전까지는 IOCP쓰레드가 아닌 쓰레드도 생성될 수 있으므로. 완료전까지 생성된 쓸모없는 패킷큐는 걸러줘야함
	jc::Vector<Int32U> iocpThreadIdList = g_cNet.GetGroup()->GetIocp()->GetWorkThreadIdList();
	auto fnContained = [&iocpThreadIdList](const IOCPThreadId$CommandQueuePair& pair)
	{
		return iocpThreadIdList.Exist(pair.key_);
	};
	iocpThreadAccessCommandQueueList_ = iocpThreadAccessCommandQueueList_.Extension().Filter(fnContained).ToVector();
	packetQueueCount_ = iocpThreadAccessCommandQueueList_.Size();
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommandSynchronizer::AllocateCommandQueue()
{
	auto fnAllocator = [this](const IOCPThreadId$CommandQueuePair& pair)
	{
		CommandQueueHolder* pHolder = pair.value_;
		CommandQueue* pReceiverQueue = dbg_new CommandQueue(pHolder->initialCapacity_);
		CommandQueue* pSwapQueue = dbg_new CommandQueue(pHolder->initialCapacity_);

		pHolder->pLock_ = dbg_new jc::NormalLock;
		pHolder->pMemPool_ = dbg_new jc::CIndexedMemoryPool();
		pHolder->pQueue_ = pReceiverQueue;
		swapCommandQueue_.PushBack(pSwapQueue);
	};
	iocpThreadAccessCommandQueueList_.ForEach(fnAllocator);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CommandSynchronizer::Finalize()
{
	for (int i = 0; i < packetQueueCount_; ++i)
	{
		CommandQueueHolder* pIOCPPacketQueueHolder = iocpThreadAccessCommandQueueList_[i].value_;
		CommandQueue* pQueue;

		// 미처리 데이터 삭제
		{
			pQueue = pIOCPPacketQueueHolder->pQueue_;
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

		JC_DELETE_SAFE(pIOCPPacketQueueHolder->pQueue_);
		JC_DELETE_SAFE(pIOCPPacketQueueHolder->pLock_);
		JC_DELETE_SAFE(pIOCPPacketQueueHolder->pMemPool_);
		JC_DELETE_SAFE(swapCommandQueue_[i]);
	}
	iocpThreadAccessCommandQueueList_.Clear();
	swapCommandQueue_.Clear();
	packetQueueCount_ = 0;

	CommandHolder::FreeAllObjects();
}
