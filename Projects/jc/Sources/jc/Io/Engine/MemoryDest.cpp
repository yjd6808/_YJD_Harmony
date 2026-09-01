#include "jc/Io/Engine/MemoryDest.h"
#include "jc/Math.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
MemoryDest::MemoryDest(_s64 _memoryLimit, _u32 _initCapacity)
	: memoryLimit_(_memoryLimit)
	, initCapacity_(_initCapacity)
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
	if (_expectedBytes > memoryLimit_)
	{
		lastError_ = ieMemoryLimit;
		return false;
	}

	const _u32 cap = _expectedBytes > 0
		? Math::Min((_u64)_expectedBytes, (_u64)0xFFFFFFFFLL)
		: initCapacity_;
	spBuffer_ = MakeShared<MemoryStream>(cap);
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
	if ((_s64)spBuffer_->GetSize() + _len > memoryLimit_)
	{
		lastError_ = ieMemoryLimit;
		return false;
	}
	spBuffer_->Write(_pBuffer, (_u32)_len);
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
	spBuffer_ = nullptr;	// 실패/취소 시 미회수 버퍼 즉시 해제 (Shutdown 규약의 큐 배수와 짝)
}

//////////////////////////////////////////////////////////////////////////////////////////
MemoryStreamPtr MemoryDest::TakeResult()
{
	MemoryStreamPtr spOut = spBuffer_;
	spBuffer_ = nullptr;
	return spOut;
}

NS_END
