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
	IOCPTaskResult() {}
	IOCPTaskResult(const IOCPTaskResult<T>& other) { Value = other.Value; }
	IOCPTaskResult(IOCPTaskResult<T>&& other) noexcept { Value = JCore::Move(other.Value); }
	~IOCPTaskResult() { Destroy(); }

	template <typename... Args>
	void Construct(Args&&... args) { JCore::Memory::PlacementNew(Value, JCore::Forward<Args>(args)...); }
	void Destroy() { JCore::Memory::PlacementDelete(Value); }

	union { T Value; };
	JCore::AtomicBool Success;
	JCore::AtomicInt32U ErrorCode;
};


template <>
struct IOCPTaskResult<void>
{
	JCore::AtomicBool Success;
	JCore::AtomicInt32U ErrorCode;
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
	IOCPTaskAbstract(IOCP* iocp)
		: m_WaitHandle(false)
		, m_eState(IOCPTaskState::eInitialized)
		, m_pIocp(iocp)
	{}

	virtual ~IOCPTaskAbstract() = default;
	virtual void Start() = 0;

	bool IsReady() { return m_eState >= IOCPTaskState::eReady; }
	int GetState() { return m_eState; }
protected:
	JCore::AutoResetEvent m_WaitHandle;
	JCore::AtomicInt m_eState;

	IOCP* m_pIocp;

	IOCPTaskAbstractPtr m_spContinuousTask;
	JCore::SpinLock m_lkContinuousTaskLock;
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
	IOCPTask(IOCP* iocp, const TFnTask& fnTask, const TFnTask& fnFinally)
		: IOCPTaskAbstract(iocp)
		, m_fnTask(fnTask)
		, m_fnFinally(fnFinally)
	{}

	static IOCPTaskAbstractPtr Create(IOCP* iocp, const TFnTask& task, const TFnTask& fnFinally) {
		return JCore::MakeShared<TIOCPTask>(iocp, task, fnFinally);
	}

	template <typename... Args>
	static IOCPTaskAbstractPtr Run(IOCP* iocp, const TFnTask& fnTask, const TFnTask& fnFinally = nullptr, Args&&... args) {
		TIOCPTaskPtr spTask = Create(iocp, fnTask, fnFinally);

		spTask->m_spResult = JCore::MakeShared<TResult>();

		if constexpr (!IsVoidTask) {
			static_assert(JCore::IsConstructible_v<T, Args...>, "... [Task<T>] cannot construct T");
			spTask->m_spResult->Construct(JCore::Forward<Args>(args)...);
		} else {
			static_assert(sizeof...(Args) == 0, "... [Task<void>] too many arguments");
		}

		spTask->Start();
		return spTask;
	}

	
	void Start() override {

		DebugAssertMsg(m_spResult != nullptr, "TaskResult가 생성되어있지 않습니다.");

		TResult* pResult = m_spResult.GetPtr();

		pResult->ErrorCode = 0;
		pResult->Success = false;
		m_eState = IOCPTaskState::eRunning;
		TIOCPOverlappedTask* pOverlapped = dbg_new TIOCPOverlappedTask(m_pIocp, this->Shared());

		if (m_pIocp->Post(0, NULL, pOverlapped) == FALSE) {
			DebugAssertMsg(false, "Task::Start Failed");
			pOverlapped->Release();
			m_eState = IOCPTaskState::eFinished;
			pResult->Success = false;
			pResult->ErrorCode = IOCPTASK_FAILED;
		}
	}

	TResult& Wait(int timeout = JCORE_INFINITE) {

		if (m_eState >= IOCPTaskState::eReady)
			return *m_spResult;

		Int32U iErrorCode;
		const bool bWait = m_WaitHandle.Wait(timeout, &iErrorCode);

		if (!bWait) {
			m_spResult->Success = false;
			m_spResult->ErrorCode = iErrorCode;
		}

		return *m_spResult;
	}

	IOCPTaskAbstractPtr ContinuousWith(const TFnTask& fnTask, const TFnTask& fnFinally = nullptr) {
		TIOCPTaskPtr spTask = Create(m_pIocp, fnTask, fnFinally);
		spTask->m_spResult = m_spResult;
		if (m_eState >= IOCPTaskState::eFinished) {
			spTask->Start();
			return spTask;
		}

		{
			JCORE_LOCK_GUARD(m_lkContinuousTaskLock);
			m_spContinuousTask = spTask;
		}
		return spTask;
	}


private:
	static constexpr bool IsVoidTask = JCore::IsVoidType_v<T>;

	TFnTask m_fnTask;
	TFnTask m_fnFinally;
	TResultPtr m_spResult;
	template <typename> friend class IOCPOverlappedTask;
};

NS_JNET_END