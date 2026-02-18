/*
 * 작성자: 윤정도
 * 생성일: 4/15/2023 6:21:23 AM
 * =====================
 *
 */


#pragma once


#include <jc/Sync/ManualResetEvent.h>
#include <jc/Sync/SpinLock.h>

#include <jnet/IOCP/IOCP.h>
#include <jnet/IOCPOverlapped/IOCPOverlappedTask.h>

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
		value_ = jc::Move(_other.value_);
	}

	~IOCPTaskResult()
	{
		Destroy();
	}

	template <typename... Args>
	void Construct(Args&&... _args)
	{
		jc::Memory::PlacementNew(value_, jc::Forward<Args>(_args)...);
	}

	void Destroy()
	{
		jc::Memory::PlacementDelete(value_);
	}

	union
	{
		T value_;
	};

	jc::AtomicBool success_;
	jc::AtomicInt32U errorCode_;
};


template <>
struct IOCPTaskResult<void>
{
	jc::AtomicBool success_;
	jc::AtomicInt32U errorCode_;
};


template <typename T>
class IOCPOverlappedTask;
class IOCPTaskAbstract;
using IOCPTaskAbstractPtr = jc::SharedPtr<IOCPTaskAbstract>;

template <typename T>
using FnTask = std::function<void(IOCPTaskResult<T>&)>;

class IOCPTaskAbstract : public jc::MakeSharedFromThis<IOCPTaskAbstract>
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
	jc::AutoResetEvent waitHandle_;
	jc::AtomicInt state_;

	IOCP* iocp_;

	IOCPTaskAbstractPtr continuousTask_;
	jc::SpinLock continuousTaskLock_;
};


template <typename T>
class IOCPTask : public IOCPTaskAbstract
{
	using TIOCPTask = IOCPTask<T>;
	using TIOCPTaskPtr = jc::SharedPtr<IOCPTask<T>>;
	using TIOCPOverlappedTask = IOCPOverlappedTask<T>;

	using TResult = IOCPTaskResult<T>;
	using TResultPtr = jc::SharedPtr<TResult>;
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
		return jc::MakeShared<TIOCPTask>(_pIocp, _task, _fnFinally);
	}

	template <typename... Args>
	static IOCPTaskAbstractPtr Run(IOCP* _pIocp, const TFnTask& _fnTask, const TFnTask& _fnFinally = nullptr, Args&&... _args)
	{
		TIOCPTaskPtr pTask = Create(_pIocp, _fnTask, _fnFinally);

		pTask->result_ = jc::MakeShared<TResult>();

		if constexpr (!IsVoidTask)
		{
			static_assert(jc::IsConstructible_v<T, Args...>, "... [Task<T>] cannot construct T");
			pTask->result_->Construct(jc::Forward<Args>(_args)...);
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
		jc_assert_msg(result_ != nullptr, "TaskResult가 생성되어있지 않습니다.");

		TResult* pResult = result_.GetPtr();

		pResult->errorCode_ = 0;
		pResult->success_ = false;
		state_ = IOCPTaskState::eRunning;
		TIOCPOverlappedTask* pOverlapped = dbg_new TIOCPOverlappedTask(iocp_, this->Shared());

		if (!iocp_->Post(0, NULL, pOverlapped))
		{
			jc_assert_msg(false, "Task::Start Failed");
			pOverlapped->Release();
			state_ = IOCPTaskState::eFinished;
			pResult->success_ = false;
			pResult->errorCode_ = IOCPTASK_FAILED;
		}
	}

	TResult& Wait(int _timeout = JC_INFINITE)
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
			JC_LOCK_GUARD(continuousTaskLock_);
			continuousTask_ = pTask;
		}
		return pTask;
	}

	void Call()
	{
		if (fnTask_)
		{
			fnTask_(*result_);
		}
	}

	void CallFinally()
	{
		if (fnFinally_)
		{
			fnFinally_(*result_);
		}
	}

private:
	static constexpr bool IsVoidTask = jc::IsVoidType_v<T>;

	TFnTask fnTask_;
	TFnTask fnFinally_;
	TResultPtr result_;
	template <typename> friend class IOCPOverlappedTask;
};

NS_END
