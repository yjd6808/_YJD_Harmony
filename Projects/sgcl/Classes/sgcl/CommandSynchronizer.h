/*
 * 작성자: 윤정도
 * 생성일: 5/9/2023 3:29:07 AM
 * =====================
 * 여러 IOCP 쓰레드에서 수신한 커맨드들을 코코스 쓰레드로 받아서 처리하기 위함
 */


#pragma once

#include <sgcl/Core.h>

class CommandSynchronizer final : public jc::SingletonPointer<CommandSynchronizer>
{
	struct CommandHolder;
	using CommandQueue = jc::ArrayQueue<CommandHolder*>;

	struct CommandQueueHolder
	{
		CommandQueueHolder()
		: initialCapacity_(0)
		, pQueue_(nullptr)
		, pMemPool_(nullptr)
		, pLock_(nullptr)
		{
		}

		CommandQueueHolder(int _capacity)
		: initialCapacity_(_capacity)
		, pQueue_(nullptr)
		, pMemPool_(nullptr)
		, pLock_(nullptr)
		{
		}

		int initialCapacity_;
		CommandQueue* pQueue_;
		jc::CIndexedMemoryPool* pMemPool_; // 데이터를 반환해줄 메모리풀
		jc::NormalLock* pLock_;
	};

	struct CommandHolder : jc::ObjectPool<CommandHolder>
	{
		CommandHolder()
		: pCommand_(nullptr)
		, pMemPool_(nullptr)
		, listenerType_(ClientConnectServerType::Max)
		, pSender_(nullptr)
		{
		}

		CommandHolder(ClientConnectServerType_t _listenerType, jnet::Session* _pSender, jnet::ICommand* _pCopy);
		~CommandHolder() override;

		jnet::ICommand* pCommand_; // 무조건 위쪽에 위치해야 캐스팅이 정상적으로 동작함
		jc::CIndexedMemoryPool* pMemPool_; // 데이터를 돌려놓을 메모리풀(홀더 해제를 메인쓰레드에서 수행하기 때문에 포인터정보가 필요함)
		ClientConnectServerType_t listenerType_;
		jnet::Session* pSender_;
	};

	using IOCPThreadId$CommandQueuePair = jc::Pair<Int32U, CommandQueueHolder*>; // IOCP 쓰레드의 ID와 커맨드큐 페어
	using IOCPThreadId$CommandQueuePairList = jc::Vector<IOCPThreadId$CommandQueuePair>;

	friend class TSingleton;
	CommandSynchronizer();
	~CommandSynchronizer();

	void FilterUnusedCommandQueue(); // 안쓰는 패킷큐는 해제 (메인 쓰레드에서 생성된 큐의 경우 쓸일이 없으므로)
	void AllocateCommandQueue();
	void Finalize();
	static CommandQueueHolder RegisterPacketQueueAddress(int _initCapacity);

public:
	void EnqueueCommand(ClientConnectServerType_t _listenerType, jnet::Session* _pSender, jnet::ICommand* _pCmd);
	void Initialize();
	void ProcessCommands();

private:
	int packetQueueCount_;
	jc::Vector<CommandQueue*> swapCommandQueue_;
	IOCPThreadId$CommandQueuePairList iocpThreadAccessCommandQueueList_;

	static bool RegistrationEnd;
	static thread_local CommandQueueHolder tlsCommandQueueHolder;
};
