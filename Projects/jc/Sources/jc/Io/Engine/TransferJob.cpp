#include "jc/Io/Engine/TransferJob.h"
#include "jc/Assert.h"

NS_JC_BEGIN

namespace
{
	//////////////////////////////////////////////////////////////////////////////////////
	void FailJob(const TransferJobPtr& _spJob, IoError _error, const IoErrorDetail& _detail)
	{
		_spJob->error_.Store((int)_error);
		_spJob->state_.Store(isFailed);
		_spJob->chanError_ = _detail.channelError_;
		if (_detail.httpStatus_ != 0)
			_spJob->sourceInfo_.httpStatus_ = _detail.httpStatus_;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	void Terminal(const TransferJobPtr& _spJob, int _state, IoError _error, _s32 _chanError)
	{
		_spJob->error_.Store((int)_error);
		_spJob->state_.Store(_state);
		if (_chanError != 0)
			_spJob->chanError_ = _chanError;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 유일한 복사 루프 (C안 심장) — 재사용 버퍼 주입(잡당 할당 0회), _shutdown은 엔진 소유 플래그
void PumpTransferJob(const TransferJobPtr& _spJob, _byte* _pWorkBuf, _s32 _workBufLen, const Atomic<bool>& _shutdown)
{
	jc_assert(_spJob != nullptr && _pWorkBuf != nullptr && _workBufLen > 0);

	if (!_spJob->TrySetState(isPending, isOpening))
		return;

	// 1. 원천 열기 + 메타 수신 (HTTP 상태/헤더 포함)
	IoSourceInfo sinfo;
	if (!_spJob->spSource_->Open(OUT sinfo))
	{
		_spJob->sourceInfo_ = sinfo;
		FailJob(_spJob, _spJob->spSource_->GetLastError(), _spJob->spSource_->GetLastErrorDetail());
		return;
	}
	_spJob->sourceInfo_ = sinfo;
	_spJob->totalBytes_.Store(sinfo.totalBytes_);

	// 2. 목적지 열기 (메모리 제한 1차 검사 / .part 파일 생성)
	if (!_spJob->spDest_->Open(sinfo.totalBytes_))
	{
		_spJob->spSource_->Close();
		FailJob(_spJob, _spJob->spDest_->GetLastError(), IoErrorDetail{});
		return;
	}

	_spJob->TrySetState(isOpening, isTransferring);

	const _s32 readUnit = (_spJob->policy_.readUnit_ > 0 && _spJob->policy_.readUnit_ <= _workBufLen)
		? _spJob->policy_.readUnit_
		: _workBufLen;

	bool completed = false;
	IoError failError = ieNone;
	IoErrorDetail failDetail{};

	for (;;)
	{
		if (_spJob->cancelRequested_.Load())
		{
			_spJob->spSource_->Close();
			_spJob->spDest_->Abort();
			Terminal(_spJob, isCancelled, ieCancelledByUser, 0);
			return;
		}
		if (_shutdown.Load())
		{
			_spJob->spSource_->Close();
			_spJob->spDest_->Abort();
			Terminal(_spJob, isCancelled, ieShutdown, 0);
			return;
		}

		const int n = _spJob->spSource_->Read(_pWorkBuf, readUnit);
		if (n == 0)
		{
			completed = true;	// EOF — 정상 완료
			break;
		}
		if (n < 0)
		{
			failError = _spJob->spSource_->GetLastError();
			failDetail = _spJob->spSource_->GetLastErrorDetail();
			break;
		}

		if (!_spJob->spDest_->Write(_pWorkBuf, n))
		{
			failError = _spJob->spDest_->GetLastError();
			break;
		}
		_spJob->transferredBytes_.Add(n);
	}

	_spJob->spSource_->Close();

	if (!completed)
	{
		_spJob->spDest_->Abort();
		FailJob(_spJob, failError == ieNone ? ieReadFailed : failError, failDetail);
		return;
	}

	// 3. 성공 마감 — FileDest: .part → Move (원자적 커밋)
	if (!_spJob->spDest_->Commit())
	{
		const IoError commitErr = _spJob->spDest_->GetLastError();
		FailJob(_spJob, commitErr == ieNone ? ieWriteFailed : commitErr, IoErrorDetail{});
		return;
	}

	Terminal(_spJob, isCompleted, ieNone, 0);

	// 4. 메모리 결과 회수 (MemoryDest만 버퍼 반환, 파일은 nullptr)
	_spJob->spMemoryResult_ = _spJob->spDest_->TakeResult();
}

NS_END
