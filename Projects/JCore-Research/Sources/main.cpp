#include "header.h"
#include <future>


/* 작업 결과물을 어떻게 가져올지 */
enum class TaskGetValueStrategy
{
	Move,
	Copy
};

/* 작업 결과물 대기 수행결과 */
enum class TaskWaitResult
{
	Success,			// 올바르게 작업이 수행되었고 데이터를 가져옴
	NotInitialized,		// 작업 컨텍스트가 설정되지 않은경우
	ValueNotExist,		// 올바르게 작업이 수행되었으나 데이터가 존재하지 않음
	Cancelled,			// 작업이 중단됨.
	
};

template <typename T>
struct TaskResult
{


	union UnionHolder
	{
		UnionHolder() {}
		~UnionHolder() {}

		T Value;
		bool Held = false;

		void Destroy() {
			if (Held) {
				Memory::PlacementDelete(Value);
				Held = false;
			}
		}
	};

	struct StructHolder
	{
		T Value;
		bool Held = false;

		void Destroy() { /* 할거 없음 */ }
	};


	using TResult = TaskResult<T>;
	using THolder = Conditional_t<is_default_constructible_v<T>, StructHolder, UnionHolder>;

	~TaskResult() {
		Holder.Destroy();
	}

	TResult& operator=(T&& t) {
		Holder.Value = Move(t);
		Holder.Held = true;
		return *this;
	}

	bool TryGet(JCORE_OUT T& v, TaskGetValueStrategy getValueStrategy) {
		if (!Holder.Held) 
			return false;
		if (getValueStrategy == TaskGetValueStrategy::Copy) {
			v = Holder.Value;
			return true;
		}
		if (getValueStrategy == TaskGetValueStrategy::Move) {
			v = Move(Holder.Value);
			Holder.Held = false;
			return true;
		}
		return false;
	}

	THolder Holder;
};


template <typename T>
class TaskContextImpl;
class JCORE_NOVTABLE TaskContext
{
public:
	enum State
	{
		eRunningWait,
		eRunning,
		eFinished,
		eCancelled
	};

	TaskContext()
		: m_eState(eRunningWait)
	{}

	virtual ~TaskContext() = default;
	virtual void RunImpl() = 0;
	virtual bool IsResultTask() = 0;

	void Run() {
		
		{
			NormalLockGuard guard(m_CtxLock);
			if (m_eState == eCancelled) {
				return;
			}
			m_eState = eRunning;
			RunImpl();
		}
		m_eState = eFinished;
		m_CtxCondVar.NotifyAll();
	}

	void Cancel() {
		{
			NormalLockGuard guard(m_CtxLock);
			m_eState = eCancelled;
		}
		m_CtxCondVar.NotifyAll();
	}

	template <typename T>
	TaskWaitResult Wait(JCORE_OUT T* v, TaskGetValueStrategy getValueStrategy) {
		int eState;
		TaskWaitResult eResult = TaskWaitResult::Success;

		NormalLockGuard guard(m_CtxLock);
		m_CtxCondVar.Wait(guard, [this, &eState] {
			eState = m_eState;
			return eState == eFinished || eState == eCancelled;
		});

		if (eState == eCancelled) 
			return TaskWaitResult::Cancelled;

		if constexpr (!IsVoidType_v<T>) {
			DebugAssert(IsResultTask());
			auto impl = static_cast<TaskContextImpl<T>*>(this);
			if (!impl->m_Result.TryGet(*v, getValueStrategy)) {
				eResult = TaskWaitResult::ValueNotExist;
			}
		}

		return eResult;
	}

	template <typename T>
	TaskWaitResult TryWait(JCORE_OUT T* v, TaskGetValueStrategy getValueStrategy) {
		
		TaskWaitResult eResult = TaskWaitResult::Success;
		NormalLockGuard guard(m_CtxLock);
		const int eState = m_eState;

		if (eState == eCancelled)
			return TaskWaitResult::Cancelled;

		if (eState != eFinished) {
			return TaskWaitResult::ValueNotExist;
		}

		if (IsResultTask() && v != nullptr) {
			auto impl = static_cast<TaskContextImpl<T>*>(this);

			if (!impl->m_Result.TryGet(*v, getValueStrategy)) {
				eResult = TaskWaitResult::ValueNotExist;
			}
		}

		return eResult;
	}


private:
	ConditionVariable m_CtxCondVar;
	NormalLock m_CtxLock;
	AtomicInt m_eState;
};

template <typename Ret>
class TaskContextImpl : TaskContext
{
public:
	TaskContextImpl(Func<Ret>&& task)
		: m_fnTask(Move(task))
	{}

	void RunImpl() override { m_Result = m_fnTask(); }
	bool IsResultTask() override { return true; }

private:
	TaskResult<Ret> m_Result;
	Func<Ret> m_fnTask;

	friend class TaskContext;
};

template <>
class TaskContextImpl<void> : TaskContext
{
public:
	TaskContextImpl(Func<void>&& task)
		: m_fnTask(Move(task))
	{}

	void RunImpl() override { m_fnTask(); }
	bool IsResultTask() override { return false; }

	Func<void> m_fnTask;
};

using TaskContextPtr = SharedPtr<TaskContext>;

template <typename T>
struct Task
{
	using TTask = Task<T>;

	Task() = default;
	Task(const TaskContextPtr& context) : m_spContext(context) {}
	Task(const TTask& task) { m_spContext = task.m_spContext; }
	Task(TTask&& task) noexcept { m_spContext = Move(task.m_spContext); }

	TTask& operator=(const TTask& task) { m_spContext = task.m_spContext; return *this; }
	TTask& operator=(TTask&& task) noexcept { m_spContext = Move(task.m_spContext); return *this; }

	bool Wait(JCORE_OUT T* v = nullptr, TaskGetValueStrategy getValueStrategy = TaskGetValueStrategy::Move, JCORE_OUT TaskWaitResult* waitResult = nullptr) const {
		if (m_spContext == nullptr) {
			if (waitResult) *waitResult = TaskWaitResult::NotInitialized;
			return false;
		}

		const TaskWaitResult eResult = m_spContext->Wait(v, getValueStrategy);
		if (waitResult) *waitResult = eResult;
		if (eResult != TaskWaitResult::Success) {
			return false;
		}

		return true;
	}

	bool TryWait(JCORE_OUT T* v = nullptr, TaskGetValueStrategy getValueStrategy = TaskGetValueStrategy::Move, JCORE_OUT TaskWaitResult* waitResult = nullptr) const {
		if (m_spContext == nullptr) {
			if (waitResult) *waitResult = TaskWaitResult::NotInitialized;
			return false;
		}

		const TaskWaitResult eResult = m_spContext->TryWait(v, getValueStrategy);
		if (waitResult) *waitResult = eResult;
		if (eResult != TaskWaitResult::Success) {
			return false;
		}

		return true;
	}

private:
	TaskContextPtr m_spContext;
};

template <>
struct Task<void>
{
	using TTask = Task<void>;

	Task() = default;
	Task(const TaskContextPtr& context) : m_spContext(context) {}
	Task(const TTask& task) { m_spContext = task.m_spContext; }
	Task(TTask&& task) noexcept { m_spContext = Move(task.m_spContext); }

	TTask& operator=(const TTask& task) { m_spContext = task.m_spContext; return *this; }
	TTask& operator=(TTask&& task) noexcept { m_spContext = Move(task.m_spContext); return *this; }

	bool Wait(JCORE_OUT TaskWaitResult* waitResult = nullptr) const {
		if (m_spContext == nullptr) {
			if (waitResult) *waitResult = TaskWaitResult::NotInitialized;
			return false;
		}
		void* v = nullptr;
		const TaskWaitResult eResult = m_spContext->Wait<void>(v, TaskGetValueStrategy::Move);

		if (waitResult) *waitResult = eResult;
		if (eResult != TaskWaitResult::Success) {
			return false;
		}

		return true;
	}

private:
	TaskContextPtr m_spContext;
};

class A
{
	A(int b) {}
};

/*class ThreadSafeTaskQueue
{
public:
	void Enqueue(const TaskContextPtr& taskCtx) {
		LOCK_GUARD(m_Lock);
		m_Queue.Enqueue(taskCtx);
	}

	bool TryDequeue(JCORE_OUT TaskContextPtr& task) {
		LOCK_GUARD(m_Lock);
		if (!m_Queue.IsEmpty()) {
			task = m_Queue.Front();
			m_Queue.Dequeue();
			return true;
		}

		return false;
	}

	int Count() {
		LOCK_GUARD(m_Lock);
		return m_Queue.Size();
	}
private:
	ArrayQueue<TaskContextPtr> m_Queue;
	NormalLock m_Lock;
};*/

using TaskQueue = ArrayQueue<TaskContextPtr>;
class TaskThread : public RunnableThread
{
public:
	TaskThread(ConditionVariable& poolCv, ConditionVariable& joinCv, NormalLock& poolLock, bool& poolStopFlag, TaskQueue& poolTaskQueue)
		: RunnableThread()
		, m_PoolCondVar(poolCv)
		, m_JoinCondVar(joinCv)
		, m_PoolLock(poolLock)
		, m_bPoolStopFlag(poolStopFlag)
		, m_qPoolWaitingTasks(poolTaskQueue)
		, m_bJoinWait(false)
	{}

	void CancelRunningTask() {
		NormalLockGuard guard(m_Lock);
		if (m_spRunningTask != nullptr) {
			m_spRunningTask->Cancel();
			m_spRunningTask = nullptr;
		}
	}
	bool IsJoinWait() { return m_bJoinWait; }
protected:
	bool PreStart() override { return true; }
	bool PreStop() override { return true; }
	void WorkerThread() override {

		int iCount = 0;
		bool bExit = false;
		TaskContextPtr spRunningTask;

		for (;;) {
			if (spRunningTask != nullptr) {
				{
					NormalLockGuard guard(m_Lock);
					m_spRunningTask = spRunningTask;
				}
				spRunningTask->Run();
				spRunningTask = nullptr;
			}

			NormalLockGuard guard(m_PoolLock);
			m_PoolCondVar.Wait(guard, [this, &iCount, &bExit] {
				iCount = m_qPoolWaitingTasks.Size();
				bExit = m_bPoolStopFlag || m_bThisStopFlag;
				return bExit || iCount > 0;
			});

			if (bExit) 
				break;

			m_qPoolWaitingTasks.TryDequeue(spRunningTask);
		}

		Console::WriteLine("쓰레드 %d 종료됨", GetThreadId());

		m_bJoinWait = true;
		m_JoinCondVar.NotifyOne();
	}

private:
	// 쓰레드풀 변수 참조용
	ConditionVariable& m_PoolCondVar;
	ConditionVariable& m_JoinCondVar;
	NormalLock& m_PoolLock;
	bool& m_bPoolStopFlag;
	TaskQueue& m_qPoolWaitingTasks;


	NormalLock m_Lock;
	AtomicBool m_bJoinWait;
	AtomicBool m_bThisStopFlag;	// 쓰레드 개별 중지 요청
	TaskContextPtr m_spRunningTask;
};

using TaskThreadPtr = SharedPtr<TaskThread>;

class ThreadPool
{
public:
	enum State
	{
		eRunning,
		eJoinWait,
		eJoined
	};

	ThreadPool(int poolSize)
		: m_vThreads(poolSize)
		, m_eState(eRunning)
	{
		for (int i = 0; i < poolSize; ++i) {
			TaskThreadPtr spThread = MakeShared<TaskThread>(m_CondVar, m_JoinCondVar, m_Lock, m_bStopFlag, m_qWaitingTasks);
			m_vThreads.PushBack(spThread);
			spThread->Start();
		}
	}

	template <typename Callable, typename... Args>
	auto Run(Callable&& callable, Args&&... args) {
		using _Callable = RemoveQulifierReference_t<Callable>;
		using _Ret = CallableSignatureReturn_t<_Callable>;

		Func<_Ret> fn = std::bind(Forward<Callable>(callable), Forward<Args>(args)...);
		TaskContextPtr spContext;
		{
			NormalLockGuard guard(m_Lock);
			spContext = MakeShared<TaskContextImpl<_Ret>>(Move(fn));
			m_qWaitingTasks.Enqueue(spContext);
		}
		m_CondVar.NotifyOne();
		return Task<_Ret>{ spContext };
	}

	// TODO: 쓰레드풀 리사이즈 필요시 구현
	bool Resize(int size) {
		return false;
	}

	void Join() {
		m_eState = eJoinWait;
		{
			NormalLockGuard guard(m_Lock);
			m_bStopFlag = true;

			while (!m_qWaitingTasks.IsEmpty()) {
				m_qWaitingTasks.Front()->Cancel();
				m_qWaitingTasks.Dequeue();
			}

			// for (int i = 0; i < m_vThreads.Size(); ++i) {
			// 		m_vThreads[i]->CancelRunningTask();	// 이미 실행중인 작업은 완료될때까지 기다리도록 하는게 나을듯?
			// }

		}
		m_CondVar.NotifyAll();

		{
			NormalLockGuard guard(m_Lock);
			for (int i = 0; i < m_vThreads.Size(); ++i) {
				TaskThread* pThread = m_vThreads[i].GetPtr();
				m_JoinCondVar.Wait(guard, [pThread] { return pThread->IsJoinWait(); });
				pThread->Join();
			}
			m_vThreads.Clear();
		}
		
		m_eState = eJoined;
	}

	int WaitingTaskCount() {
		NormalLockGuard guard(m_Lock);
		return m_qWaitingTasks.Size();
	}
private:
	Vector<TaskThreadPtr> m_vThreads;
	TaskQueue m_qWaitingTasks;
	ConditionVariable m_CondVar;
	ConditionVariable m_JoinCondVar;
	NormalLock m_Lock;
	AtomicInt m_eState;
	bool m_bStopFlag;
};


ConditionVariable cv;
NormalLock n;

int main() {
	Console::SetSize(800, 600);
	Random::EngineInitialize();

	ThreadPool pool(3);

	Task<int> t1 = pool.Run([](int a, int b) {
		Console::WriteLine("태스크1");
		Thread::Sleep(3000);
		return 100;
	}, 100, 200);

	Task<void> t2 = pool.Run([](int a, int b) {
		Console::WriteLine("태스크2");
		Thread::Sleep(6000);
	}, 100, 200);

	Task<void> taskArr[20];

	for (int i = 3; i <= 8; ++i) {
		taskArr[i - 3] = pool.Run([=](int a, int b) {
			Console::WriteLine("태스크%d", i);
			Thread::Sleep(6000);
		}, 100, 200);
	}

	Thread th1{ [=](void*) {
		int r;
		if (!t1.Wait(&r)) {
			Console::WriteLine("t1 작업 취소됨");
		} else {
			Console::WriteLine("t1 작업 완료 %d", r);
		}
	} };

	Thread th2{ [=](void*) {
		if (!t2.Wait()) {
			Console::WriteLine("t2 작업 취소됨");
		} else {
			Console::WriteLine("t1 작업 작업완료");
		}
	} };

	Thread th3{ [=](void*) {
		for (int i = 0; i <= 5; ++i) {
			TaskWaitResult r;
			taskArr[i].Wait(&r);
			Console::WriteLine("대기 결과 %d", r);
		}
	}};
	
	
	for (;;) {
		String s = Console::ReadLine();
		if (s == "a") {
			pool.Join();
			break;
		}
	}

	th1.Join();
	th2.Join();
	th3.Join();
	return 0;
}
