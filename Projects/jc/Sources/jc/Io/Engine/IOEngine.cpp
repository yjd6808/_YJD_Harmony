#include "jc/IO/Engine/IOEngine.h"
#include "jc/Threading/ThreadPool.h"
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
	//   폴백(스테이징) 경로에서만 lazy 확보 — 제로카피 경로가 잡으면 할당 자체가 없다 (P1-3)
	struct WorkBuffer
	{
		_byte* pBuffer_ = nullptr;
		_s32 length_ = 0;
		~WorkBuffer() { JC_DELETE_ARRAY_SAFE(pBuffer_); }

		_byte* Ensure(_s32 _need)
		{
			if (length_ < _need)
			{
				JC_DELETE_ARRAY_SAFE(pBuffer_);
				pBuffer_ = dbg_new _byte[_need];
				length_ = _need;
			}
			return pBuffer_;
		}
	};
}

//////////////////////////////////////////////////////////////////////////////////////////
IOEngine::~IOEngine()
{
	Shutdown();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IOEngine::Initialize(const IOEngineConfig& _config)
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
void IOEngine::Shutdown()
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
IOJobPtr IOEngine::MakeJob(const IOSourcePtr& _spSource, const IODestPtr& _spDest,
	const String& _uri, const TransferPolicy& _policy, const IOCallback& _callback)
{
	IOJobPtr spJob = MakeShared<IOJob>();
	spJob->handle_ = idProvider_.Acquire();
	spJob->uri_ = _uri;
	spJob->policy_ = _policy;
	spJob->callback_ = _callback;
	spJob->spSource_ = _spSource;
	spJob->spDest_ = _spDest;
	spJob->spDest_->Bind(spJob->handle_);		// FileDest의 .part.{handle} 이름 확정
	return spJob;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOEngine::ExecuteOnWorker(const IOJobPtr& _spJob)
{
	{
		LockGuard<NormalLock> guard(activeLock_);
		activeJobs_.Insert(_spJob->handle_, _spJob);
	}
	runningWorkerJobs_.Increment();
	pPool_->Run([this, _spJob] { ExecuteJob(this, _spJob); });
}

//////////////////////////////////////////////////////////////////////////////////////////
IOHandle IOEngine::Submit(const IOSourcePtr& _spSource, const IODestPtr& _spDest,
	const String& _uri, const TransferPolicy& _policy, const IOCallback& _callback)
{
	jc_assert(initialized_.Load());
	if (shuttingDown_.Load())
	{
		// P0-5: 발급 핸들을 그대로 반환 — 콜백 발화 핸들과 관제 핸들의 불일치 해소
		return FailImmediate(ieShutdown, _uri, _callback);
	}

	IOJobPtr spJob = MakeJob(_spSource, _spDest, _uri, _policy, _callback);
	ExecuteOnWorker(spJob);
	return spJob->handle_;
}

//////////////////////////////////////////////////////////////////////////////////////////
IOHandle IOEngine::FailImmediate(IOError _error, const String& _uri, const IOCallback& _callback)
{
	IOJobPtr spJob = MakeShared<IOJob>();
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
IOResult IOEngine::RunSync(const IOSourcePtr& _spSource, const IODestPtr& _spDest,
	const String& _uri, const TransferPolicy& _policy)
{
	jc_assert(initialized_.Load());

	IOJobPtr spJob;
	if (shuttingDown_.Load())
	{
		spJob = MakeShared<IOJob>();
		spJob->handle_ = idProvider_.Acquire();
		spJob->uri_ = _uri;
		spJob->error_.Store((int)ieShutdown);
		spJob->state_.Store(isFailed);
	}
	else
	{
		spJob = MakeJob(_spSource, _spDest, _uri, _policy, IOCallback());
	}

	{
		LockGuard<NormalLock> guard(activeLock_);
		activeJobs_.Insert(spJob->handle_, spJob);
	}
	runningWorkerJobs_.Increment();

	// 접수 스레드 인라인 실행 — 동기 API는 워커를 거치지 않는다
	{
		static thread_local WorkBuffer t_work;
		_byte* pBuf = nullptr;
		_s32 bufLen = 0;
		if (spJob->NeedsStagingBuffer())
		{
			bufLen = config_.readUnit_;
			pBuf = t_work.Ensure(bufLen);
		}
		PumpJob(spJob, pBuf, bufLen);
	}

	runningWorkerJobs_.Decrement();
	Finalize(spJob);

	{
		LockGuard<NormalLock> guard(activeLock_);
		activeJobs_.Remove(spJob->handle_);
	}
	return spJob->result_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOEngine::ExecuteJob(IOEngine* _pSelf, const IOJobPtr& _spJob)
{
	static thread_local WorkBuffer t_work;
	_byte* pBuf = nullptr;
	_s32 bufLen = 0;
	if (_spJob->NeedsStagingBuffer())
	{
		bufLen = _pSelf->config_.readUnit_;
		pBuf = t_work.Ensure(bufLen);
	}

	_pSelf->PumpJob(_spJob, pBuf, bufLen);

	// 선점 성공한 쪽만 완료 큐 적재 — 취소 경로와 이중 적재 방지
	if (_pSelf->Finalize(_spJob))
	{
		LockGuard<NormalLock> guard(_pSelf->completedLock_);
		_pSelf->completedQueue_.PushBack(_spJob);
	}
	_pSelf->runningWorkerJobs_.Decrement();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 유일한 복사 루프 (C안 심장) — 3경로:
//   ① 소스 직독 (source.BeginRead → dest.Write) — Memory→File Save
//   ② 목적지 직접기록 (dest.BeginWrite → source.Read) — File→Memory Load, Http→Memory Get
//   ③ 스테이징 폴백 (workbuf 경유, File→File 등) — _pWorkBuf == nullptr이면 lazy 할당
void IOEngine::PumpJob(const IOJobPtr& _spJob, _byte* _pWorkBuf, _s32 _workBufLen)
{
	jc_assert(_spJob != nullptr);

	if (!_spJob->TrySetState(isPending, isOpening))
		return;

	const _s32 effUnit = _spJob->policy_.readUnit_ > 0 ? _spJob->policy_.readUnit_ : config_.readUnit_;
	const _s32 intervalMs = _spJob->policy_.progressIntervalMs_ > 0
		? _spJob->policy_.progressIntervalMs_
		: config_.progressNotifyIntervalMs_;
	_u64 lastNotifyTick = GetTickMs();

	auto notify = [&]
	{
		// P0-1: 간격 제한 진행 통지 배선 (0 이하 = 매 청크 통지)
		if (intervalMs <= 0 || GetTickMs() - lastNotifyTick >= (_u64)intervalMs)
		{
			PushProgress(_spJob);
			lastNotifyTick = GetTickMs();
		}
	};

	// 1. 원천 열기 + 메타 수신
	IOSourceInfo sinfo;
	if (!_spJob->spSource_->Open(OUT sinfo))
	{
		_spJob->error_.Store((int)_spJob->spSource_->GetLastError());
		_spJob->state_.Store(isFailed);
		_spJob->chanError_ = _spJob->spSource_->GetChannelError();
		return;
	}
	_spJob->totalBytes_.Store(sinfo.totalBytes_);

	// 2. 목적지 열기 (메모리 제한 1차 검사 / .part 파일 생성)
	if (!_spJob->spDest_->Open(sinfo.totalBytes_))
	{
		_spJob->spSource_->Close();
		_spJob->error_.Store((int)_spJob->spDest_->GetLastError());
		_spJob->state_.Store(isFailed);
		_spJob->chanError_ = _spJob->spDest_->GetChannelError();
		return;
	}

	_spJob->TrySetState(isOpening, isTransferring);

	bool completed = false;
	IOError failError = ieNone;
	_s32 failChannel = 0;

	// 스테이징 폴백용 로컬 버퍼 (필요할 때만 할당)
	_byte* pLocalBuf = nullptr;
	_s32 localLen = 0;

	for (;;)
	{
		if (_spJob->cancelRequested_.Load())
		{
			_spJob->spSource_->Close();
			_spJob->spDest_->Abort();
			_spJob->error_.Store((int)ieCancelledByUser);
			_spJob->state_.Store(isCancelled);
			break;
		}
		if (shuttingDown_.Load())
		{
			_spJob->spSource_->Close();
			_spJob->spDest_->Abort();
			_spJob->error_.Store((int)ieShutdown);
			_spJob->state_.Store(isCancelled);
			break;
		}

		// 경로 ① — 소스 직독
		_u32 avail = 0;
		if (const _byte* pSrc = _spJob->spSource_->BeginRead(avail))
		{
			if (avail == 0)
			{
				completed = true;	// EOF — 정상 완료
				break;
			}
			const _u32 n = avail < (_u32)effUnit ? avail : (_u32)effUnit;
			if (!_spJob->spDest_->Write(pSrc, n))
			{
				failError = _spJob->spDest_->GetLastError();
				failChannel = _spJob->spDest_->GetChannelError();
				break;
			}
			_spJob->spSource_->EndRead(n);
			_spJob->transferredBytes_.Add(n);
			notify();
			continue;
		}

		// 경로 ② — 목적지 직접기록 (허가량까지만 Read — 고정 버퍼 오버플로 방지)
		_u32 granted = 0;
		if (_byte* pDst = _spJob->spDest_->BeginWrite(effUnit, granted))
		{
			const int n = _spJob->spSource_->Read(pDst, (_s32)granted);
			if (n == 0)
			{
				_spJob->spDest_->EndWrite(0);
				completed = true;	// EOF — 정상 완료
				break;
			}
			if (n < 0)
			{
				_spJob->spDest_->EndWrite(0);
				failError = _spJob->spSource_->GetLastError();
				failChannel = _spJob->spSource_->GetChannelError();
				break;
			}
			if (!_spJob->spDest_->EndWrite((_u32)n))
			{
				failError = _spJob->spDest_->GetLastError();
				failChannel = _spJob->spDest_->GetChannelError();
				break;
			}
			_spJob->transferredBytes_.Add(n);
			notify();
			continue;
		}

		// 경로 ③ — 스테이징 폴백
		if (pLocalBuf == nullptr)
		{
			if (_pWorkBuf != nullptr && _workBufLen > 0)
			{
				pLocalBuf = _pWorkBuf;
				localLen = _workBufLen;
			}
			else
			{
				localLen = effUnit;
				pLocalBuf = dbg_new _byte[localLen];
			}
		}

		const int chunk = localLen < effUnit ? localLen : effUnit;
		const int n = _spJob->spSource_->Read(pLocalBuf, chunk);
		if (n == 0)
		{
			completed = true;	// EOF — 정상 완료
			break;
		}
		if (n < 0)
		{
			failError = _spJob->spSource_->GetLastError();
			failChannel = _spJob->spSource_->GetChannelError();
			break;
		}

		if (!_spJob->spDest_->Write(pLocalBuf, n))
		{
			failError = _spJob->spDest_->GetLastError();
			failChannel = _spJob->spDest_->GetChannelError();
			break;
		}
		_spJob->transferredBytes_.Add(n);
		notify();
	}

	_spJob->spSource_->Close();

	if (pLocalBuf != nullptr && pLocalBuf != _pWorkBuf)
		JC_DELETE_ARRAY_SAFE(pLocalBuf);

	if (!completed)
	{
		_spJob->spDest_->Abort();
		_spJob->error_.Store((int)(failError == ieNone ? ieReadFailed : failError));
		_spJob->state_.Store(isFailed);
		_spJob->chanError_ = failChannel;
		return;
	}

	// 3. 성공 마감 — FileDest: .part → Move (원자적 커밋)
	if (!_spJob->spDest_->Commit())
	{
		const IOError commitErr = _spJob->spDest_->GetLastError();
		_spJob->error_.Store((int)(commitErr == ieNone ? ieWriteFailed : commitErr));
		_spJob->state_.Store(isFailed);
		_spJob->chanError_ = _spJob->spDest_->GetChannelError();
		return;
	}

	_spJob->error_.Store((int)ieNone);
	_spJob->state_.Store(isCompleted);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IOEngine::Finalize(const IOJobPtr& _spJob)
{
	if (_spJob->resultReady_.Exchange(true))
		return false;	// 이미 조립됨 — 취소 경로와 이중 완료 방지

	IOResult result;
	result.handle_ = _spJob->handle_;
	result.state_ = static_cast<IOState>(_spJob->state_.Load());
	result.error_ = static_cast<IOError>(_spJob->error_.Load());
	result.channelError_ = _spJob->chanError_;
	result.bytesTransferred_ = (_u64)_spJob->transferredBytes_.Load();
	_spJob->result_ = result;

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
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOEngine::PushProgress(const IOJobPtr& _spJob)
{
	LockGuard<NormalLock> guard(progressLock_);
	IOProgress* pExist = pendingProgress_.Find(_spJob->handle_);	// 핸들당 최신 1건 합삭
	if (pExist != nullptr)
		*pExist = MakeProgress(_spJob);
	else
		pendingProgress_.Insert(_spJob->handle_, MakeProgress(_spJob));
}

//////////////////////////////////////////////////////////////////////////////////////////
IOProgress IOEngine::MakeProgress(const IOJobPtr& _spJob) const
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
void IOEngine::SetListener(IIOListener* _pListener, bool _transferOwnership)
{
	if (listenerOwned_ && pListener_ != nullptr)
		JC_DELETE_SAFE(pListener_);
	pListener_ = _pListener;
	listenerOwned_ = _transferOwnership;
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOEngine::Pump()
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
	Vector<IOJobPtr> done;
	{
		LockGuard<NormalLock> guard(completedLock_);
		if (completedQueue_.Size() > 0)
			done = Move(completedQueue_);
	}

	for (int i = 0; i < done.Size(); ++i)
	{
		const IOJobPtr& spJob = done[i];
		const IOResult& result = spJob->result_;

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
IOState IOEngine::GetState(IOHandle _handle) const
{
	LockGuard<NormalLock> guard(activeLock_);
	IOJobPtr* pJob = activeJobs_.Find(_handle);
	if (pJob != nullptr)
		return static_cast<IOState>((*pJob)->state_.Load());
	return isNone;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool IOEngine::GetProgress(IOHandle _handle, OUT IOProgress& _out) const
{
	{
		LockGuard<NormalLock> guard(activeLock_);
		IOJobPtr* pJob = activeJobs_.Find(_handle);
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
bool IOEngine::Cancel(IOHandle _handle)
{
	IOJobPtr spJob;
	{
		LockGuard<NormalLock> guard(activeLock_);
		IOJobPtr* pJob = activeJobs_.Find(_handle);
		if (pJob == nullptr)
			return false;
		spJob = *pJob;
	}

	if (spJob->TrySetState(isPending, isCancelled))		// 미실행 선점 취소
	{
		spJob->error_.Store((int)ieCancelledByUser);
		if (Finalize(spJob))
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
void IOEngine::CancelAll()
{
	LockGuard<NormalLock> guard(activeLock_);
	auto it = activeJobs_.Begin();
	while (it.HasNext())
	{
		auto& pair = it.Next();
		IOJobPtr spJob = pair.value_;
		if (spJob->TrySetState(isPending, isCancelled))
		{
			spJob->error_.Store((int)ieCancelledByUser);
			if (Finalize(spJob))
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
int IOEngine::GetActiveCount() const
{
	LockGuard<NormalLock> guard(activeLock_);
	return activeJobs_.Size();
}

//////////////////////////////////////////////////////////////////////////////////////////
void IOEngine::GetActiveSnapshot(OUT Vector<IOProgress>& _out) const
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
