/*
 * 작성자: 윤정도
 * 생성일: 4/15/2023 6:21:23 AM
 * =====================
 *
 */


#pragma once


#include <JCore/Sync/ManualResetEvent.h>
#include <JCore/Sync/SpinLock.h>

#include <JNetwork/IOCP/IOCP.h>

#define IOCPTASK_FAILED	50000

NS_JNET_BEGIN

template <typename T>
struct IOCPTaskResult
{
	IOCPTaskResult() = default;
	IOCPTaskResult(const IOCPTaskResult<T>& _other)
	{
		value_ = _other.value_;
	}

	IOCPTaskResult(IOCPTaskResult<T>&& _other) noexcept
	{
		value_ = JCore::Move(_other.value_);
	}

	~IOCPTaskResult()
	{
		Destroy();
	}

	template <typename... Args>
	void Construct(Args&&... _args)
	{
		JCore::Memory::PlacementNew(value_, JCore::Forward<Args>(_args)...);
	}

	void Destroy()
	{
		JCore::Memory::PlacementDelete(value_);
	}

	union
	{
		T value_;
	};

	JCore::AtomicBool success_;
	JCore::AtomicInt32U errorCode_;
};


template <>
struct IOCPTaskResult<void>
{
	JCore::AtomicBool success_;
	JCore::AtomicInt32U errorCode_;
};


template <typename T>
class IOCPOverlappedTask;
class IOCPTaskAbstract;
using IOCPTaskAbstractPtr = JCore::SharedPtr<IOCPTaskAbstract>;

template <typename T>
using FnTask = std::function<void(IOCPTaskResult<T>&)>;


struct IOCPTaskState
{
	enum _
	{
		eInitialized,
		eRunning,
		eReady,
		eFinished
	};
};


class IOCPTaskAbstract : public JCore::MakeSharedFromThis<IOCPTaskAbstract>
{
public:
	IOCPTaskAbstract(IOCP* _pIocp)
		: waitHandle_(false)
		, state_(IOCPTaskState::eInitialized)
		, iocp_(_pIocp)
	{
	}

	virtual ~IOCPTaskAbstract() = default;
	virtual void Start() = 0;

	bool IsReady()
	{
		return state_ >= IOCPTaskState::eReady;
	}

	int GetState()
	{
		return state_;
	}

protected:
	JCore::AutoResetEvent waitHandle_;
	JCore::AtomicInt state_;

	IOCP* iocp_;

	IOCPTaskAbstractPtr continuousTask_;
	JCore::SpinLock continuousTaskLock_;
};


template <typename T>
class IOCPTask : public IOCPTaskAbstract
{
	using TIOCPTask = IOCPTask<T>;
	using TIOCPTaskPtr = JCore::SharedPtr<IOCPTask<T>>;
	using TIOCPOverlappedTask = IOCPOverlappedTask<T>;

	using TResult = IOCPTaskResult<T>;
	using TResultPtr = JCore::SharedPtr<TResult>;
	using TFnTask = FnTask<T>;

public:
	IOCPTask(IOCP* _pIocp, const TFnTask& _fnTask, const TFnTask& _fnFinally)
		: IOCPTaskAbstract(_pIocp)
		, fnTask_(_fnTask)
		, fnFinally_(_fnFinally)
	{
	}

	static IOCPTaskAbstractPtr Create(IOCP* _pIocp, const TFnTask& _task, const TFnTask& _fnFinally)
	{
		return JCore::MakeShared<TIOCPTask>(_pIocp, _task, _fnFinally);
	}

	template <typename... Args>
	static IOCPTaskAbstractPtr Run(IOCP* _pIocp, const TFnTask& _fnTask, const TFnTask& _fnFinally = nullptr, Args&&... _args)
	{
		TIOCPTaskPtr pTask = Create(_pIocp, _fnTask, _fnFinally);

		pTask->result_ = JCore::MakeShared<TResult>();

		if constexpr (!IsVoidTask)
		{
			static_assert(JCore::IsConstructible_v<T, Args...>, "... [Task<T>] cannot construct T");
			pTask->result_->Construct(JCore::Forward<Args>(_args)...);
		}
		else
		{
			static_assert(sizeof...(Args) == 0, "... [Task<void>] too many arguments");
		}

		pTask->Start();
		return pTask;
	}

	void Start() override
	{
		DebugAssertMsg(result_ != nullptr, "TaskResult가 생성되어있지 않습니다.");

		TResult* pResult = result_.GetPtr();

		pResult->errorCode_ = 0;
		pResult->success_ = false;
		state_ = IOCPTaskState::eRunning;
		TIOCPOverlappedTask* pOverlapped = dbg_new TIOCPOverlappedTask(iocp_, this->Shared());

		if (!iocp_->Post(0, NULL, pOverlapped))
		{
			DebugAssertMsg(false, "Task::Start Failed");
			pOverlapped->Release();
			state_ = IOCPTaskState::eFinished;
			pResult->success_ = false;
			pResult->errorCode_ = IOCPTASK_FAILED;
		}
	}

	TResult& Wait(int _timeout = JCORE_INFINITE)
	{
		if (state_ >= IOCPTaskState::eReady)
		{
			return *result_;
		}

		Int32U errorCode;
		const bool wait = waitHandle_.Wait(_timeout, &errorCode);

		if (!wait)
		{
			result_->success_ = false;
			result_->errorCode_ = errorCode;
		}

		return *result_;
	}

	IOCPTaskAbstractPtr ContinuousWith(const TFnTask& _fnTask, const TFnTask& _fnFinally = nullptr)
	{
		TIOCPTaskPtr pTask = Create(iocp_, _fnTask, _fnFinally);
		pTask->result_ = result_;
		if (state_ >= IOCPTaskState::eFinished)
		{
			pTask->Start();
			return pTask;
		}

		{
			JCORE_LOCK_GUARD(continuousTaskLock_);
			continuousTask_ = pTask;
		}
		return pTask;
	}

private:
	static constexpr bool IsVoidTask = JCore::IsVoidType_v<T>;

	TFnTask fnTask_;
	TFnTask fnFinally_;
	TResultPtr result_;
	template <typename> friend class IOCPOverlappedTask;
};

NS_JNET_END
