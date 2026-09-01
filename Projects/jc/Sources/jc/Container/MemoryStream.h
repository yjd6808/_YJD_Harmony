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
		if (_capacity > 0)
		{
			pBytes_ = dbg_new _byte[_capacity];
			capacity_ = _capacity;
			owner_ = true;
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
