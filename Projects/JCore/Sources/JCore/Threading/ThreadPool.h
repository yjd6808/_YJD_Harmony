/*
 * 작성자: 윤정도
 * 생성일: 6/8/2023 2:49:34 PM
 * =====================
 *
 */


#pragma once

#include <JCore/TypeTraits.h>
#include <JCore/Sync/ConditionVariable.h>
#include <JCore/Container/ArrayQueue.h>
#include <JCore/Container/Vector.h>
#include <JCore/Threading/RunnableThread.h>

#define TASKPOOL_DEBUG_LOG_ENABLE	0

#if TASKPOOL_DEBUG_LOG_ENABLE
	#include <JCore/Utils/Console.h>
	#define TASKPOOL_LOG(fmt, ...) Console::WriteLine(fmt, __VA_ARGS__)
#else
	#define TASKPOOL_LOG(fmt, ...)
#endif

NS_JC_BEGIN
 
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
	using THolder = Conditional_t<IsDefaultConstructiable_v<T>, StructHolder, UnionHolder>;

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
		, m_DebugName(0)
	{}

	const char* ToStateString(int state);
	virtual ~TaskContext() = default;
	virtual void RunImpl() = 0;
	virtual bool IsResultTask() = 0;

	void Run();
	void Cancel();
	void SetDebugName(const String& s) { m_DebugName = s; }

	template <typename T>
	TaskWaitResult Wait(JCORE_OUT T* v, TaskGetValueStrategy getValueStrategy) {
		int eState;
		TaskWaitResult eResult = TaskWaitResult::Success;

		TASKPOOL_LOG("%s Wait Step Begin", m_DebugName.Source());
		NormalLockGuard guard(m_CtxLock);
		m_CtxCondVar.Wait(guard, [this, &eState] {
			eState = m_eState;
			TASKPOOL_LOG("%s Wait State: %s", m_DebugName.Source(), ToStateString(eState));
			return eState == eFinished || eState == eCancelled;
		});

		TASKPOOL_LOG("%s Wait Step End", m_DebugName.Source());
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

	AtomicInt GetState() { return m_eState; }
private:
	ConditionVariable m_CtxCondVar;
	NormalLock m_CtxLock;
	AtomicInt m_eState;
	String m_DebugName;
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

struct TaskBase 
{
	~TaskBase()
	{
		JCORE_PASS;
	}
	TaskContext::State GetContextState() { 
		if (m_spContext == nullptr) { DebugAssert(false); return TaskContext::State::eFinished; }
		return (TaskContext::State)m_spContext->GetState().Load(); 
	}
	void SetDebugName(const String& name) { 
		if (m_spContext == nullptr) { DebugAssert(false); return; }
		m_spContext->SetDebugName(name); 
	}
protected:
	TaskBase() = default;
	TaskBase(const TaskContextPtr& context) : m_spContext(context) {}
	TaskContextPtr m_spContext;
};

template <typename T>
struct Task : TaskBase
{
	using TTask = Task<T>;

	Task() = default;
	Task(const TaskContextPtr& context) : TaskBase(context) {}
	Task(const TTask& task) { m_spContext = task.m_spContext; }
	Task(TTask&& task) noexcept { m_spContext = Move(task.m_spContext); }

	TTask& operator=(const TTask& task) { m_spContext = task.m_spContext; return *this; }
	TTask& operator=(TTask&& task) noexcept { m_spContext = Move(task.m_spContext); return *this; }

	bool Wait(JCORE_OUT T* v = nullptr, TaskGetValueStrategy getValueStrategy = TaskGetValueStrategy::Move, JCORE_OUT TaskWaitResult* waitResult = nullptr) const {
		if (!ValidateGetValueStrategy(getValueStrategy)) {
			DebugAssertMsg(false, "%d 값 가져오기 방식을 사용할 수 없는 타입입니다.", (int)getValueStrategy);
			return false;
		}

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
	static bool ValidateGetValueStrategy(TaskGetValueStrategy strategy) {
		if (strategy == TaskGetValueStrategy::Copy && !IsCopyAssignable_v<T>) {
			return false;
		}

		if (strategy == TaskGetValueStrategy::Move && !IsMoveAssignable_v<T>) {
			return false;
		}

		return true;
	}
};

template <>
struct Task<void> : TaskBase
{
	using TTask = Task<void>;

	Task() = default;
	Task(const TaskContextPtr& context) : TaskBase(context) {}
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
};

using TaskQueue = ArrayQueue<TaskContextPtr>;
class TaskThread : public RunnableThread
{
public:
	TaskThread(ConditionVariable& poolCv, ConditionVariable& joinCv, NormalLock& poolLock, bool& poolStopFlag, TaskQueue& poolTaskQueue, int code);
	~TaskThread() override;

	void CancelRunningTask();
	bool IsJoinWait() { return m_bJoinWait; }
protected:
	bool PreStart() override { return true; }
	bool PreStop() override { return true; }
	void WorkerThread() override;
private:
	// 쓰레드풀 변수 참조용
	ConditionVariable& m_PoolCondVar;
	ConditionVariable& m_JoinCondVar;
	NormalLock& m_PoolLock;
	bool& m_bPoolStopFlag;
	TaskQueue& m_qPoolWaitingTasks;

	NormalLock m_Lock;
	int m_iCode;
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

	ThreadPool(int poolSize);
	
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
		Task<_Ret> task(spContext);
		return task;
	}

	// TODO: 쓰레드풀 리사이즈 필요시 구현
	bool Resize(int size) {
		return false;
	}

	void Join();
	int WaitingTaskCount();
private:
	Vector<TaskThreadPtr> m_vThreads;
	TaskQueue m_qWaitingTasks;
	ConditionVariable m_CondVar;
	ConditionVariable m_JoinCondVar;
	NormalLock m_Lock;
	AtomicInt m_eState;
	bool m_bStopFlag;
};

using ThreadPoolPtr = SharedPtr<ThreadPool>;

NS_JC_END
