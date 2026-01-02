/*
 * 작성자: 윤정도
 * 생성일: 6/8/2023 2:49:34 PM
 * =====================
 *
 */


#pragma once

#include <jc/TypeTraits.h>
#include <jc/Sync/ConditionVariable.h>
#include <jc/Container/ArrayQueue.h>
#include <jc/Container/Vector.h>
#include <jc/Threading/RunnableThread.h>

#define TASKPOOL_DEBUG_LOG_ENABLE	0

#if TASKPOOL_DEBUG_LOG_ENABLE
#include <jc/Utils/Console.h>
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
	Success, // 올바르게 작업이 수행되었고 데이터를 가져옴
	NotInitialized, // 작업 컨텍스트가 설정되지 않은경우
	ValueNotExist, // 올바르게 작업이 수행되었으나 데이터가 존재하지 않음
	Cancelled, // 작업이 중단됨.
};

template <typename T>
struct TaskResult
{
	union UnionHolder
	{
		UnionHolder()
		{
		}

		~UnionHolder()
		{
		}

		T value_;
		bool held_ = false;

		void Destroy()
		{
			if (held_)
			{
				Memory::PlacementDelete(value_);
				held_ = false;
			}
		}
	};

	struct StructHolder
	{
		T value_;
		bool held_ = false;

		void Destroy()
		{
			/* 할거 없음 */
		}
	};


	using TResult = TaskResult<T>;
	using THolder = Conditional_t<IsDefaultConstructiable_v<T>, StructHolder, UnionHolder>;

	~TaskResult()
	{
		holder_.Destroy();
	}

	TResult& operator=(T&& _t)
	{
		holder_.value_ = Move(_t);
		holder_.held_ = true;
		return *this;
	}

	bool TryGet(OUT T& _v, TaskGetValueStrategy _getValueStrategy)
	{
		if (!holder_.held_)
			return false;
		if (_getValueStrategy == TaskGetValueStrategy::Copy)
		{
			_v = holder_.value_;
			return true;
		}
		if (_getValueStrategy == TaskGetValueStrategy::Move)
		{
			_v = Move(holder_.value_);
			holder_.held_ = false;
			return true;
		}
		return false;
	}

	THolder holder_;
};


template <typename T>
class TaskContextImpl;

class JC_NOVTABLE TaskContext
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
	{
	}

	const char* ToStateString(int _state);
	virtual ~TaskContext() = default;
	virtual void RunImpl() = 0;
	virtual bool IsResultTask() = 0;

	void Run();
	void Cancel();
	void SetDebugName(const String& _s) { m_DebugName = _s; }

	template <typename T>
	TaskWaitResult Wait(OUT T* _pValue, TaskGetValueStrategy _getValueStrategy)
	{
		int eState;
		TaskWaitResult eResult = TaskWaitResult::Success;

		TASKPOOL_LOG("%s Wait Step Begin", m_DebugName.Source());
		NormalLockGuard guard(m_CtxLock);
		m_CtxCondVar.Wait(guard, [this, &eState]
		{
			eState = m_eState;
			TASKPOOL_LOG("%s Wait State: %s", m_DebugName.Source(), ToStateString(eState));
			return eState == eFinished || eState == eCancelled;
		});

		TASKPOOL_LOG("%s Wait Step End", m_DebugName.Source());
		if (eState == eCancelled)
			return TaskWaitResult::Cancelled;

		if constexpr (!IsVoidType_v<T>)
		{
			jc_assert(IsResultTask());
			auto impl = static_cast<TaskContextImpl<T>*>(this);
			if (!impl->m_Result.TryGet(*_pValue, _getValueStrategy))
			{
				eResult = TaskWaitResult::ValueNotExist;
			}
		}

		return eResult;
	}

	template <typename T>
	TaskWaitResult TryWait(OUT T* _pValue, TaskGetValueStrategy _getValueStrategy)
	{
		TaskWaitResult eResult = TaskWaitResult::Success;
		NormalLockGuard guard(m_CtxLock);
		const int eState = m_eState;

		if (eState == eCancelled)
			return TaskWaitResult::Cancelled;

		if (eState != eFinished)
		{
			return TaskWaitResult::ValueNotExist;
		}

		if (IsResultTask() && _pValue != nullptr)
		{
			auto impl = static_cast<TaskContextImpl<T>*>(this);

			if (!impl->m_Result.TryGet(*_pValue, _getValueStrategy))
			{
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
	TaskContextImpl(Func<Ret>&& _task)
	: m_fnTask(Move(_task))
	{
	}

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
	TaskContextImpl(Func<void>&& _task)
	: fnTask_(Move(_task))
	{
	}

	void RunImpl() override { fnTask_(); }
	bool IsResultTask() override { return false; }

	Func<void> fnTask_;
};

using TaskContextPtr = SharedPtr<TaskContext>;

struct TaskBase
{
	~TaskBase() = default;

	TaskContext::State GetContextState()
	{
		if (pContext == nullptr)
		{
			jc_assert(false);
			return TaskContext::State::eFinished;
		}
		return (TaskContext::State)pContext->GetState().Load();
	}

	void SetDebugName(const String& _name)
	{
		if (pContext == nullptr)
		{
			jc_assert(false);
			return;
		}
		pContext->SetDebugName(_name);
	}

protected:
	TaskBase() = default;

	TaskBase(const TaskContextPtr& _context)
	: pContext(_context)
	{
	}

	TaskContextPtr pContext;
};

template <typename T>
struct Task : TaskBase
{
	using TTask = Task<T>;

	Task() = default;

	Task(const TaskContextPtr& _context)
	: TaskBase(_context)
	{
	}

	Task(const TTask& _task) { pContext = _task.pContext; }
	Task(TTask&& _task) noexcept { pContext = Move(_task.pContext); }

	TTask& operator=(const TTask& _task);
	TTask& operator=(TTask&& _task) noexcept
	{
		pContext = Move(_task.pContext);
		return *this;
	}

	bool Wait(
		OUT T* _pValue = nullptr, 
		TaskGetValueStrategy _getValueStrategy = TaskGetValueStrategy::Move,
		OUT TaskWaitResult* _pWaitResult = nullptr) const
	{
		if (!ValidateGetValueStrategy(_getValueStrategy))
		{
			jc_assert_msg(false, "%d 값 가져오기 방식을 사용할 수 없는 타입입니다.", (int)_getValueStrategy);
			return false;
		}

		if (pContext == nullptr)
		{
			if (_pWaitResult) *_pWaitResult = TaskWaitResult::NotInitialized;
			return false;
		}

		const TaskWaitResult eResult = pContext->Wait(_pValue, _getValueStrategy);
		if (_pWaitResult) *_pWaitResult = eResult;
		if (eResult != TaskWaitResult::Success)
		{
			return false;
		}

		return true;
	}

	bool TryWait(
		OUT T* _pValue = nullptr, 
		TaskGetValueStrategy _getValueStrategy = TaskGetValueStrategy::Move,
		OUT TaskWaitResult* _pWaitResult = nullptr) const
	{
		if (pContext == nullptr)
		{
			if (_pWaitResult) *_pWaitResult = TaskWaitResult::NotInitialized;
			return false;
		}

		const TaskWaitResult eResult = pContext->TryWait(_pValue, _getValueStrategy);
		if (_pWaitResult) *_pWaitResult = eResult;
		if (eResult != TaskWaitResult::Success)
		{
			return false;
		}

		return true;
	}

private:
	static bool ValidateGetValueStrategy(TaskGetValueStrategy _strategy)
	{
		if (_strategy == TaskGetValueStrategy::Copy && !IsCopyAssignable_v<T>)
		{
			return false;
		}

		if (_strategy == TaskGetValueStrategy::Move && !IsMoveAssignable_v<T>)
		{
			return false;
		}

		return true;
	}
};

template <typename T> typename Task<T>::TTask& Task<T>::operator=(const TTask& _task)
{
	pContext = _task.pContext;
	return *this;
}

template <>
struct Task<void> : TaskBase
{
	using TTask = Task<void>;

	Task() = default;

	Task(const TaskContextPtr& _context)
	: TaskBase(_context)
	{
	}

	Task(const TTask& _task) { pContext = _task.pContext; }
	Task(TTask&& _task) noexcept { pContext = Move(_task.pContext); }

	TTask& operator=(const TTask& _task)
	{
		pContext = _task.pContext;
		return *this;
	}

	TTask& operator=(TTask&& _task) noexcept
	{
		pContext = Move(_task.pContext);
		return *this;
	}

	bool Wait(OUT TaskWaitResult* _pWaitResult = nullptr) const
	{
		if (pContext == nullptr)
		{
			if (_pWaitResult) *_pWaitResult = TaskWaitResult::NotInitialized;
			return false;
		}
		void* v = nullptr;
		const TaskWaitResult eResult = pContext->Wait<void>(v, TaskGetValueStrategy::Move);

		if (_pWaitResult) *_pWaitResult = eResult;
		if (eResult != TaskWaitResult::Success)
		{
			return false;
		}

		return true;
	}
};

using TaskQueue = ArrayQueue<TaskContextPtr>;

class TaskThread : public RunnableThread
{
public:
	TaskThread(
		ConditionVariable& _poolCv, 
		ConditionVariable& _joinCv, 
		NormalLock& _poolLock, 
		int& _poolState,
		TaskQueue& _poolTaskQueue, 
		int _code);
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
	int& m_ePoolState;
	TaskQueue& m_qPoolWaitingTasks;

	NormalLock m_Lock;
	int m_iCode;
	AtomicBool m_bJoinWait;
	AtomicBool m_bThisStopFlag; // 쓰레드 개별 중지 요청
	TaskContextPtr m_spRunningTask;
};

using TaskThreadPtr = SharedPtr<TaskThread>;

class ThreadPool
{
public:
	enum State
	{
		eRunning,
		eJoinWaitAll,
		eJoinWaitOnlyRunningTask,
		eJoined
	};

	enum class JoinStrategy
	{
		WaitAllTasks, // 대기중인 작업이 완료될떄가지 기다림
		WaitOnlyRunningTask // 실행중인 작업만 기다림
	};

	ThreadPool(int _poolSize);

	template <typename Callable, typename... Args>
	auto Run(Callable&& _callable, Args&&... _args)
	{
		using CallableType = RemoveConstReference_t<Callable>;
		using ReturnType = CallableSignatureReturn_t<CallableType>;

		Func<ReturnType> fn = std::bind(Forward<Callable>(_callable), Forward<Args>(_args)...);
		TaskContextPtr context;
		{
			NormalLockGuard guard(lock_);
			if (state_ != eRunning)
			{
				jc_assert_msg(state_ == eRunning, "쓰레드풀이 작업을 실행가능한 상태가 아닙니다.");
				return Task<ReturnType>{nullptr};
			}

			context = MakeShared<TaskContextImpl<ReturnType>>(Move(fn));
			waitingTasks_.Enqueue(context);
		}
		condVar_.NotifyOne();
		Task<ReturnType> task(context);
		return task;
	}

	// TODO: 쓰레드풀 리사이즈 필요시 구현
	bool Resize(int _size)
	{
		return false;
	}

	void Join(JoinStrategy _strategy = JoinStrategy::WaitOnlyRunningTask);
	int WaitingTaskCount();

private:
	Vector<TaskThreadPtr> threads_;
	TaskQueue waitingTasks_;
	ConditionVariable condVar_;
	ConditionVariable joinCondVar_;
	NormalLock lock_;
	int state_;
};

using ThreadPoolPtr = SharedPtr<ThreadPool>;

NS_JC_END
