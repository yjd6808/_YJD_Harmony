#include "jc/IO/Memory/MemorySource.h"
#include "jc/Math.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
MemorySource::MemorySource(const MemoryStreamPtr& _spData)
	: spData_(_spData)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
MemorySource::~MemorySource()
{
	Close();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MemorySource::Open(OUT IOSourceInfo& _info)
{
	if (spData_ == nullptr)
	{
		lastError_ = ieOpenFailed;
		return false;
	}
	offset_ = 0;
	_info.totalBytes_ = spData_->GetSize();
	lastError_ = ieNone;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 MemorySource::Read(_byte* _pBuffer, _s32 _len)
{
	if (spData_ == nullptr)
	{
		lastError_ = ieReadFailed;
		return -1;
	}
	const _u32 size = spData_->GetSize();
	if (offset_ >= size)
		return 0;	// EOF

	_u32 want = _len < 0 ? 0 : (_u32)_len;
	const _u32 remain = size - offset_;
	if (want > remain)
		want = remain;
	Memory::CopyUnsafe(_pBuffer, spData_->GetData() + offset_, (int)want);
	offset_ += want;
	return (_s32)want;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MemorySource::Close()
{
	// 호출자 버퍼 — 소유권 없음, 해제하지 않음
}

//////////////////////////////////////////////////////////////////////////////////////////
const _byte* MemorySource::BeginRead(_u32& _outAvailable)
{
	if (spData_ == nullptr)
	{
		_outAvailable = 0;
		return nullptr;
	}
	const _u32 size = spData_->GetSize();
	if (offset_ >= size)
	{
		_outAvailable = 0;
		return nullptr;
	}
	_outAvailable = size - offset_;
	return spData_->GetData() + offset_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MemorySource::EndRead(_u32 _consumed)
{
	if (spData_ == nullptr)
		return;
	const _u32 remain = spData_->GetSize() - offset_;
	offset_ += Math::Min(_consumed, remain);
}

NS_END
