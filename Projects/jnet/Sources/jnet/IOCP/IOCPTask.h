/*
 * 작성자: 윤정도
 * 생성일: 4/15/2023 6:21:23 AM
 * =====================
 *
 */


#pragma once


#include "jc/Sync/ManualResetEvent.h"
#include "jc/Sync/SpinLock.h"

#include "jnet/IOCP/IOCP.h"
#include "jnet/IOCPOverlapped/IOCPOverlappedTask.h"
#include "jnet/IOCP/IOCPTaskAbstract.h"

#define IOCPTASK_FAILED	50000

NS_JNET_BEGIN

template <typename T>
struct IOCPTaskResult
{
	IOCPTaskResult()
	{
		
	}

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


class IOCPOverlappedTask;

template <typename T>
using FnTask = std::function<void(IOCPTaskResult<T>&)>;

template <typename T>
class IOCPTask : public IOCPTaskAbstract
{
public:
	using TIOCPTask = IOCPTask<T>;
	using TIOCPTaskPtr = jc::SharedPtr<IOCPTask<T>>;

	using TResult = IOCPTaskResult<T>;
	using TFnTask = FnTask<T>;

public:
	IOCPTask(IOCP* _pIocp, const TFnTask& _fnTask, const TFnTask& _fnFinally)
	: IOCPTaskAbstract(_pIocp)
	, fnTask_(_fnTask)
	, fnFinally_(_fnFinally)
	{
	}
	~IOCPTask()
	{
	}

	template <typename... Args>
	static IOCPTaskAbstractPtr Create(IOCP* _pIocp, const TFnTask& _task, const TFnTask& _fnFinally = nullptr, Args&&... _args)
	{
		TIOCPTaskPtr pTask = jc::MakeShared<TIOCPTask>(_pIocp, _task, _fnFinally);

		if constexpr (!IsVoidTask)
		{
			static_assert(jc::IsConstructible_v<T, Args...>, "... [Task<T>] cannot construct T");
			pTask->result_.Construct(jc::Forward<Args>(_args)...);
		}
		else
		{
			static_assert(sizeof...(Args) == 0, "... [Task<void>] too many arguments");
		}

		return pTask;
	}

	void Start() override
	{
		result_.errorCode_ = 0;
		result_.success_ = false;
		state_ = IOCPTaskState::eRunning;
		IOCPOverlappedTask* pOverlapped = dbg_new IOCPOverlappedTask(iocp_, this->Shared());

		if (!iocp_->Post(0, NULL, pOverlapped))
		{
			jc_assert_msg(false, "Task::Start Failed");
			pOverlapped->Release();
			state_ = IOCPTaskState::eFinished;
			result_.success_ = false;
			result_.errorCode_ = IOCPTASK_FAILED;
		}
	}

	TResult& Wait(int _timeout = JC_INFINITE)
	{
		if (state_ >= IOCPTaskState::eReady)
		{
			return result_;
		}

		_u32 errorCode;
		const bool wait = waitHandle_.Wait(_timeout, &errorCode);

		if (!wait)
		{
			result_.success_ = false;
			result_.errorCode_ = errorCode;
		}

		return result_;
	}

	IOCPTaskAbstractPtr ContinuousWith(const TFnTask& _fnTask, const TFnTask& _fnFinally = nullptr)
	{
		if (pContinuousTaskLock_ == nullptr)
			pContinuousTaskLock_ = dbg_new jc::NormalLock();


		TIOCPTaskPtr pTask = Create(iocp_, _fnTask, _fnFinally);
		pTask->result_ = result_;

		JC_LOCK_GUARD(*pContinuousTaskLock_);
		pContinuousTask_ = pTask;
		if (state_ >= IOCPTaskState::eFinished)
		{
			pTask->Start();
			return pTask;
		}

		return pTask;
	}

	virtual void Call() override
	{
		if (fnTask_)
		{
			fnTask_(result_);
		}
	}

	virtual void CallFinally() override
	{
		if (fnFinally_)
		{
			fnFinally_(result_);
		}
	}

	virtual void OnFailed(_u32 _errorCode)
	{
		result_.success_ = false;
		result_.errorCode_ = _errorCode;
	}

	const TResult& GetResult() const { return result_; }
private:
	static constexpr bool IsVoidTask = jc::IsVoidType_v<T>;

	TFnTask fnTask_;
	TFnTask fnFinally_;
	TResult result_;

	friend class IOCPOverlappedTask;
};

NS_END
