#pragma once

#include "jc/Namespace.h"
#include "jc/Type.h"
#include "jc/Define.h"
#include "jc/Debug/New.h"
#include "jc/Memory.h"
#include "jc/Math.h"
#include "jc/Primitives/SmartPtr.h"

using _byte = _u8;

NS_JC_BEGIN

class MemoryStream
{
public:
	explicit MemoryStream(_u32 _capacity)
	{
		owner_ = true;		// 빈 버퍼도 소유 버퍼 — 이후 Write 시 자동 확장 (non-owner는 3인자 생성자 전용)
		if (_capacity > 0)
		{
			pBytes_ = dbg_new _byte[_capacity];
			capacity_ = _capacity;
		}
	}

	MemoryStream(_byte* _pBytes, _u32 _len, bool _owner)
	{
		pBytes_ = _pBytes;
		capacity_ = _len;
		writeOffset_ = 0;
		owner_ = _owner;
	}

	~MemoryStream()
	{
		Free();
	}

	MemoryStream(const MemoryStream&) = delete;
	MemoryStream& operator=(const MemoryStream&) = delete;
	MemoryStream(MemoryStream&&) = delete;
	MemoryStream& operator=(MemoryStream&&) = delete;

	void Write(const void* _p, _u32 _len)
	{
		ExpandAllocateIfNeeded(writeOffset_ + _len);
		Memory::CopyUnsafe(pBytes_ + writeOffset_, _p, (int)_len);
		writeOffset_ += _len;
	}

	_u32 Read(void* _pOut, _u32 _len)
	{
		_u32 readable = Math::Min(_len, writeOffset_ - readOffset_);
		if (readable > 0)
			Memory::CopyUnsafe(_pOut, pBytes_ + readOffset_, (int)readable);
		readOffset_ += readable;
		return readable;
	}

	_byte* GetData() const { return pBytes_; }
	_u32 GetSize() const { return writeOffset_; }
	_u32 GetCapacity() const { return capacity_; }
	_u32 GetReadOffset() const { return readOffset_; }
	_u32 GetWriteOffset() const { return writeOffset_; }
	bool IsOwner() const { return owner_; }

	// 제로카피 읽기 — 내부 버퍼 직접 노출 (반환 후 ConsumeRead로 확정)
	const _byte* GetReadPtr() const { return pBytes_ + readOffset_; }
	_u32 GetReadable() const { return writeOffset_ - readOffset_; }
	void ConsumeRead(_u32 _len)
	{
		readOffset_ += Math::Min(_len, writeOffset_ - readOffset_);
	}

	// 제로카피 쓰기 — 내부 버퍼 직접 노출 (반환 후 CommitWrite로 확정)
	_byte* GetWritePtr() const { return pBytes_ + writeOffset_; }
	void CommitWrite(_u32 _len)
	{
		jc_assert(writeOffset_ + _len <= capacity_);
		writeOffset_ += _len;
	}

	void ExpandAllocateIfNeeded(_u32 _need)
	{
		if (_need <= capacity_)
			return;
		if (!owner_)
		{
			jc_assert_msg(false, "Non-owner MemoryStream capacity exceeded (%u > %u)", _need, capacity_);
			return;
		}
		_u32 newCap = Math::Max(capacity_ * 2, _need);
		if (newCap == 0)
			newCap = _need;
		_byte* pNew = dbg_new _byte[newCap];
		if (pBytes_ != nullptr && writeOffset_ > 0)
			Memory::CopyUnsafe(pNew, pBytes_, (int)writeOffset_);
		if (owner_)
			JC_DELETE_ARRAY_SAFE(pBytes_);
		pBytes_ = pNew;
		capacity_ = newCap;
		owner_ = true;
	}

	void Free()
	{
		if (owner_)
			JC_DELETE_ARRAY_SAFE(pBytes_);
		pBytes_ = nullptr;
		capacity_ = 0;
		writeOffset_ = 0;
		readOffset_ = 0;
	}

private:
	_byte* pBytes_ = nullptr;
	_u32 capacity_ = 0;
	_u32 writeOffset_ = 0;
	_u32 readOffset_ = 0;
	bool owner_ = false;
};

using MemoryStreamPtr = SharedPtr<MemoryStream>;

NS_END
