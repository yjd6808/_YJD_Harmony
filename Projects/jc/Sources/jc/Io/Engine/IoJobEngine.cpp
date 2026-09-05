#include "jc/IO/Engine/IOJobEngine.h"
#include "jc/IO/IOResult.h"
#include "jc/Threading/Thread.h"
#include "jc/Debug/New.h"
#include "jc/Assert.h"

#include <Windows.h>

NS_JC_BEGIN

namespace
{
	//////////////////////////////////////////////////////////////////////////////////////
	inline _u64 GetTickMs()
	{
		return ::GetTickCount64();
	}

	// 워커별 재사용 청크 버퍼 (스레드 종료 시 자동 해제 — 잡당 dbg_new 256KB 제거)
	struct WorkBuffer
	{
		_byte* pBuffer_ = nullptr;
		_s32 length_ = 0;
		~WorkBuffer() { JC_DELETE_ARRAY_SAFE(pBuffer_); }
	};
}

//////////////////////////////////////////////////////////////////////////////////////////
IOJobEngine::~IOJobEngine()
{
	Shutdown();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IOJobEngine::Initialize(const IOJobEngineConfig& _config)
{
	jc_assert(!initialized_.Load());
	config_ = _config;

	if (config_.pSharedPool_ != nullptr)
	{
		pPool_ = config_.pSharedPool_;
		poolOwned_ = false;
	}
	else
	{
		pPool_ = dbg_new ThreadPool(config_.workerCount_);
		poolOwned_ = true;
	}

	if (config_.completedProgressKeep_ > 0)
		ring_.Resize(0);

	initialized_.Store(true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOJobEngine::Shutdown()
{
	if (!initialized_.Exchange(false))
		return;

	shuttingDown_.Store(true);
	CancelAll();

	while (runningWorkerJobs_.Load() > 0)
		Thread::Sleep(1);

	{
		LockGuard<NormalLock> guard(completedLock_);
		completedQueue_.Clear(true);			// 큐 배수 — 미회수 결과 폐기
	}
	{
		LockGuard<NormalLock> guard(progressLock_);
		pendingProgress_.Clear();
	}
	{
		LockGuard<NormalLock> guard(activeLock_);
		activeJobs_.Clear();
	}
	{
		LockGuard<NormalLock> guard(ringLock_);
		ring_.Clear(true);
		ringHead_ = 0;
	}

	if (poolOwned_ && pPool_ != nullptr)
	{
		pPool_->Join(ThreadPool::JoinStrategy::WaitOnlyRunningTask);
		JC_DELETE_SAFE(pPool_);
	}
	pPool_ = nullptr;

	if (listenerOwned_ && pListener_ != nullptr)
		JC_DELETE_SAFE(pListener_);
	pListener_ = nullptr;

	shuttingDown_.Store(false);
}

//////////////////////////////////////////////////////////////////////////////////////////
TransferJobPtr IOJobEngine::MakeJob(const IOSourcePtr& _spSource, const IODestPtr& _spDest,
	const String& _uri, const TransferPolicy& _policy, const IOCallback& _callback, bool _isHttp)
{
	TransferJobPtr spJob = MakeShared<TransferJob>();
	spJob->handle_ = idProvider_.Acquire();
	spJob->uri_ = _uri;
	spJob->policy_ = _policy;
	spJob->callback_ = _callback;
	spJob->isHttp_ = _isHttp;
	spJob->spSource_ = _spSource;
	spJob->spDest_ = _spDest;
	spJob->spDest_->Bind(spJob->handle_);		// FileDest의 .part.{handle} 이름 확정
	return spJob;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOJobEngine::ExecuteOnWorker(const TransferJobPtr& _spJob)
{
	{
		LockGuard<NormalLock> guard(activeLock_);
		activeJobs_.Insert(_spJob->handle_, _spJob);
	}
	runningWorkerJobs_.Increment();
	pPool_->Run([this, _spJob] { ExecuteJob(this, _spJob); });
}

//////////////////////////////////////////////////////////////////////////////////////////
IOHandle IOJobEngine::Submit(const IOSourcePtr& _spSource, const IODestPtr& _spDest,
	const String& _uri, const TransferPolicy& _policy, const IOCallback& _callback, bool _isHttp)
{
	jc_assert(initialized_.Load());
	if (shuttingDown_.Load())
	{
		FailImmediate(ieShutdown, _uri, _callback);
		return InvalidIOHandle;
	}

	TransferJobPtr spJob = MakeJob(_spSource, _spDest, _uri, _policy, _callback, _isHttp);
	ExecuteOnWorker(spJob);
	return spJob->handle_;
}

//////////////////////////////////////////////////////////////////////////////////////////
IOHandle IOJobEngine::FailImmediate(IOError _error, const String& _uri, const IOCallback& _callback)
{
	TransferJobPtr spJob = MakeShared<TransferJob>();
	spJob->handle_ = idProvider_.Acquire();
	spJob->uri_ = _uri;
	spJob->callback_ = _callback;
	spJob->error_.Store((int)_error);
	spJob->state_.Store(isFailed);
	Finalize(spJob);

	{
		LockGuard<NormalLock> guard(activeLock_);
		activeJobs_.Insert(spJob->handle_, spJob);
	}
	{
		LockGuard<NormalLock> guard(completedLock_);
		completedQueue_.PushBack(spJob);
	}
	return spJob->handle_;
}

//////////////////////////////////////////////////////////////////////////////////////////
IOResultPtr IOJobEngine::RunSync(const IOSourcePtr& _spSource, const IODestPtr& _spDest,
	const String& _uri, const TransferPolicy& _policy, bool _isHttp)
{
	jc_assert(initialized_.Load());

	TransferJobPtr spJob;
	if (shuttingDown_.Load())
	{
		spJob = MakeShared<TransferJob>();
		spJob->handle_ = idProvider_.Acquire();
		spJob->uri_ = _uri;
		spJob->error_.Store((int)ieShutdown);
		spJob->state_.Store(isFailed);
	}
	else
	{
		spJob = MakeJob(_spSource, _spDest, _uri, _policy, nullptr, _isHttp);
	}

	{
		LockGuard<NormalLock> guard(activeLock_);
		activeJobs_.Insert(spJob->handle_, spJob);
	}
	runningWorkerJobs_.Increment();

	// 접수 스레드 인라인 실행 — 동기 API는 워커를 거치지 않는다
	{
		static thread_local WorkBuffer t_work;
		const int need = config_.readUnit_;
		if (t_work.length_ < need)
		{
			JC_DELETE_ARRAY_SAFE(t_work.pBuffer_);
			t_work.pBuffer_ = dbg_new _byte[need];
			t_work.length_ = need;
		}
		PumpTransferJob(spJob, t_work.pBuffer_, t_work.length_, shuttingDown_);
	}

	runningWorkerJobs_.Decrement();
	Finalize(spJob);

	IOCallback cb = spJob->callback_;	// 동기 완료 콜백은 접수 스레드에서 즉시 호출
	if (cb)
		cb(*spJob->spResult_);

	{
		LockGuard<NormalLock> guard(activeLock_);
		activeJobs_.Remove(spJob->handle_);
	}
	return spJob->spResult_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOJobEngine::ExecuteJob(IOJobEngine* _pSelf, const TransferJobPtr& _spJob)
{
	// 워커별 재사용 청크 버퍼
	static thread_local WorkBuffer t_work;
	{
		const int need = _pSelf->config_.readUnit_;
		if (t_work.length_ < need)
		{
			JC_DELETE_ARRAY_SAFE(t_work.pBuffer_);
			t_work.pBuffer_ = dbg_new _byte[need];
			t_work.length_ = need;
		}
	}

	PumpTransferJob(_spJob, t_work.pBuffer_, t_work.length_, _pSelf->shuttingDown_);

	_pSelf->Finalize(_spJob);
	{
		LockGuard<NormalLock> guard(_pSelf->completedLock_);
		_pSelf->completedQueue_.PushBack(_spJob);
	}
	_pSelf->runningWorkerJobs_.Decrement();
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOJobEngine::Finalize(const TransferJobPtr& _spJob)
{
	IOResultPtr spRes = MakeShared<IOResult>();
	spRes->handle_ = _spJob->handle_;
	spRes->state_ = static_cast<IOState>(_spJob->state_.Load());
	spRes->error_ = static_cast<IOError>(_spJob->error_.Load());
	spRes->detail_.channelError_ = _spJob->chanError_;

	if (_spJob->isHttp_)
	{
		spRes->http_.status_ = _spJob->sourceInfo_.httpStatus_;
		if (_spJob->sourceInfo_.spHeaders_ != nullptr)
			spRes->http_.headers_ = *_spJob->sourceInfo_.spHeaders_;
	}
	spRes->detail_.httpStatus_ = _spJob->sourceInfo_.httpStatus_;

	if (_spJob->state_.Load() == isCompleted && _spJob->spMemoryResult_ != nullptr)
	{
		spRes->data_ = _spJob->spMemoryResult_;
		if (_spJob->isHttp_)
			spRes->http_.body_ = spRes->data_;	// REST 바디 — data_와 동일 버퍼
	}
	_spJob->spMemoryResult_ = nullptr;
	_spJob->spResult_ = spRes;

	// 완료 진행률 링 캐시 기록 (유한 상한 — 무한 성장 맵 대체, R9)
	{
		LockGuard<NormalLock> guard(ringLock_);
		if (config_.completedProgressKeep_ > 0)
		{
			if (ring_.Size() < config_.completedProgressKeep_)
				ring_.PushBack(MakeProgress(_spJob));
			else
			{
				ring_[ringHead_] = MakeProgress(_spJob);
				ringHead_ = (ringHead_ + 1) % config_.completedProgressKeep_;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOJobEngine::PushProgress(const TransferJobPtr& _spJob)
{
	LockGuard<NormalLock> guard(progressLock_);
	IOProgress* pExist = pendingProgress_.Find(_spJob->handle_);	// 핸들당 최신 1건 합삭
	if (pExist != nullptr)
		*pExist = MakeProgress(_spJob);
	else
		pendingProgress_.Insert(_spJob->handle_, MakeProgress(_spJob));
}

//////////////////////////////////////////////////////////////////////////////////////////
IOProgress IOJobEngine::MakeProgress(const TransferJobPtr& _spJob) const
{
	IOProgress progress;
	progress.handle_ = _spJob->handle_;
	progress.state_ = static_cast<IOState>(_spJob->state_.Load());
	progress.totalBytes_ = _spJob->totalBytes_.Load();
	progress.transferredBytes_ = _spJob->transferredBytes_.Load();
	progress.uri_ = _spJob->uri_;
	return progress;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOJobEngine::SetListener(IIOListener* _pListener, bool _transferOwnership)
{
	if (listenerOwned_ && pListener_ != nullptr)
		JC_DELETE_SAFE(pListener_);
	pListener_ = _pListener;
	listenerOwned_ = _transferOwnership;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOJobEngine::Pump()
{
	// 1. 진행 통지 (합삭된 최신 스냅샷만)
	Vector<IOProgress> progress;
	{
		LockGuard<NormalLock> guard(progressLock_);
		if (pendingProgress_.Size() > 0)
		{
			auto it = pendingProgress_.Begin();
			while (it.HasNext())
			{
				auto& pair = it.Next();
				progress.PushBack(pair.value_);
			}
			pendingProgress_.Clear();
		}
	}
	for (int i = 0; i < progress.Size(); ++i)
	{
		if (pListener_ != nullptr)
			pListener_->OnProgress(progress[i]);
	}

	// 2. 완료 큐 swap 소비 — 콜백 먼저, 리스너 다음 (현행 순서 보존)
	Vector<TransferJobPtr> done;
	{
		LockGuard<NormalLock> guard(completedLock_);
		if (completedQueue_.Size() > 0)
			done = Move(completedQueue_);
	}

	for (int i = 0; i < done.Size(); ++i)
	{
		const TransferJobPtr& spJob = done[i];
		const IOResult& result = *spJob->spResult_;

		if (spJob->callback_)
			spJob->callback_(result);

		if (pListener_ != nullptr)
		{
			switch (static_cast<IOState>(spJob->state_.Load()))
			{
			case isCompleted: pListener_->OnCompleted(result); break;
			case isCancelled: pListener_->OnCancelled(result); break;
			case isFailed: pListener_->OnFailed(result); break;
			default: break;
			}
		}

		{
			LockGuard<NormalLock> guard(activeLock_);
			activeJobs_.Remove(spJob->handle_);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
IOState IOJobEngine::GetState(IOHandle _handle) const
{
	LockGuard<NormalLock> guard(activeLock_);
	TransferJobPtr* pJob = activeJobs_.Find(_handle);
	if (pJob != nullptr)
		return static_cast<IOState>((*pJob)->state_.Load());
	return isNone;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IOJobEngine::GetProgress(IOHandle _handle, OUT IOProgress& _out) const
{
	{
		LockGuard<NormalLock> guard(activeLock_);
		TransferJobPtr* pJob = activeJobs_.Find(_handle);
		if (pJob != nullptr)
		{
			_out = MakeProgress(*pJob);
			return true;
		}
	}
	// 완료 링 캐시 — 최신부터 역방향 검색
	{
		LockGuard<NormalLock> guard(ringLock_);
		for (int i = 0; i < ring_.Size(); ++i)
		{
			const int idx = ring_.Size() < config_.completedProgressKeep_
				? ring_.Size() - 1 - i
				: (ringHead_ - 1 - i + config_.completedProgressKeep_ * 2) % config_.completedProgressKeep_;
			if (idx < 0 || idx >= ring_.Size())
				continue;
			if (ring_[idx].handle_ == _handle)
			{
				_out = ring_[idx];
				return true;
			}
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IOJobEngine::Cancel(IOHandle _handle)
{
	TransferJobPtr spJob;
	{
		LockGuard<NormalLock> guard(activeLock_);
		TransferJobPtr* pJob = activeJobs_.Find(_handle);
		if (pJob == nullptr)
			return false;
		spJob = *pJob;
	}

	if (spJob->TrySetState(isPending, isCancelled))		// 미실행 선점 취소
	{
		spJob->error_.Store((int)ieCancelledByUser);
		Finalize(spJob);
		{
			LockGuard<NormalLock> guard(completedLock_);
			completedQueue_.PushBack(spJob);
		}
		return true;
	}

	spJob->cancelRequested_.Store(true);				// 실행 중 협조 취소
	return !spJob->IsTerminal();
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOJobEngine::CancelAll()
{
	LockGuard<NormalLock> guard(activeLock_);
	auto it = activeJobs_.Begin();
	while (it.HasNext())
	{
		auto& pair = it.Next();
		TransferJobPtr spJob = pair.value_;
		if (spJob->TrySetState(isPending, isCancelled))
		{
			spJob->error_.Store((int)ieCancelledByUser);
			Finalize(spJob);
			{
				LockGuard<NormalLock> completedGuard(completedLock_);
				completedQueue_.PushBack(spJob);
			}
		}
		else
		{
			spJob->cancelRequested_.Store(true);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
int IOJobEngine::GetActiveCount() const
{
	LockGuard<NormalLock> guard(activeLock_);
	return activeJobs_.Size();
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOJobEngine::GetActiveSnapshot(OUT Vector<IOProgress>& _out) const
{
	LockGuard<NormalLock> guard(activeLock_);
	auto it = activeJobs_.Begin();
	while (it.HasNext())
	{
		auto& pair = it.Next();
		_out.PushBack(MakeProgress(pair.value_));
	}
}

NS_END
