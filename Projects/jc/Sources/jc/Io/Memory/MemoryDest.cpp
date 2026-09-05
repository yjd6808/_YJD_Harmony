#include "jc/IO/Memory/MemoryDest.h"
#include "jc/Math.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
MemoryDest::MemoryDest(const MemoryStreamPtr& _spOut, _s64 _memoryLimit)
	: spBuffer_(_spOut)
	, memoryLimit_(_memoryLimit)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
MemoryDest::~MemoryDest()
{
	Abort();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MemoryDest::Open(_s64 _expectedBytes)
{
	if (spBuffer_ == nullptr)
	{
		lastError_ = ieWriteFailed;
		return false;
	}
	if (_expectedBytes > memoryLimit_)
	{
		lastError_ = ieMemoryLimit;
		return false;
	}

	// P1-3: 크기 확정 시 사전 예약 — 전송 중 재할당 제거
	//   고정 버퍼(non-owner)는 Open에서 실패시키지 않는다 — 전송이 잔여분까지 채운 뒤
	//   ieBufferTooSmall로 끝나야 bytesTransferred_ 재시도 힌트가 살아남는다 (§1 계약 2)
	if (_expectedBytes > 0)
	{
		_u64 want = (_u64)_expectedBytes;
		if (want > (_u64)0xFFFFFFFFLL)
			want = (_u64)0xFFFFFFFFLL;
		if (want > spBuffer_->GetCapacity() && spBuffer_->IsOwner())
		{
			if ((_s64)want > memoryLimit_)
			{
				lastError_ = ieMemoryLimit;
				return false;
			}
			spBuffer_->ExpandAllocateIfNeeded((_u32)want);
		}
	}
	lastError_ = ieNone;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MemoryDest::Write(const _byte* _pBuffer, _s32 _len)
{
	if (spBuffer_ == nullptr)
	{
		lastError_ = ieWriteFailed;
		return false;
	}
	if (_len < 0)
	{
		lastError_ = ieWriteFailed;
		return false;
	}
	const _u64 size = spBuffer_->GetSize();
	if (size + (_u64)_len > (_u64)memoryLimit_)
	{
		lastError_ = ieMemoryLimit;
		return false;
	}
	if (size + (_u64)_len > spBuffer_->GetCapacity() && !spBuffer_->IsOwner())
	{
		// §1 계약 2: 고정 버퍼(raw/non-owner) 용량 부족 — bytesTransferred_가 재시도 힌트
		lastError_ = ieBufferTooSmall;
		return false;
	}
	spBuffer_->Write(_pBuffer, (_u32)_len);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
_byte* MemoryDest::BeginWrite(_u32 _maxLen, OUT _u32& _outGranted)
{
	_outGranted = 0;
	if (spBuffer_ == nullptr || _maxLen == 0)
		return nullptr;
	const _u32 size = spBuffer_->GetSize();
	_u32 cap = spBuffer_->GetCapacity();
	if (size >= cap)
	{
		if (!spBuffer_->IsOwner())
			return nullptr;
		_u64 want = (_u64)size + _maxLen;
		if (want > (_u64)memoryLimit_)
		{
			if ((_u64)size >= (_u64)memoryLimit_)
				return nullptr;
			want = (_u64)memoryLimit_;
		}
		if (want > (_u64)0xFFFFFFFFLL)
			want = (_u64)0xFFFFFFFFLL;
		spBuffer_->ExpandAllocateIfNeeded((_u32)want);
		cap = spBuffer_->GetCapacity();
		if (size >= cap)
			return nullptr;
	}
	// 허가량 = 잔여 용량과 요청의 최소값 — 펌프는 이만큼까지만 Read (오버플로 방지)
	_outGranted = _maxLen < cap - size ? _maxLen : cap - size;
	return spBuffer_->GetWritePtr();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MemoryDest::EndWrite(_u32 _written)
{
	if (spBuffer_ == nullptr)
	{
		lastError_ = ieWriteFailed;
		return false;
	}
	const _u32 size = spBuffer_->GetSize();
	if (_written > spBuffer_->GetCapacity() - size)
	{
		lastError_ = ieWriteFailed;
		return false;
	}
	spBuffer_->CommitWrite(_written);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MemoryDest::Commit()
{
	return spBuffer_ != nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MemoryDest::Abort()
{
	// 호출자 버퍼 — 소유권이 없으므로 파기하지 않고 잔류 데이터를 그대로 둔다
}

NS_END
