#include "PooledMemoryChunk.h"

USING_NS_JC;

PooledMemoryChunk::PooledMemoryChunk(const MemoryPoolAbstractPtr& _allocator, int _requestSize)
: allocator_(_allocator)
, buffer_(nullptr)
, requestSize_(_requestSize)
, realSize_(0)
{
	jc_assert_msg(allocator_ != nullptr, "PooledMemoryChunk - allocator is nullptr");

	if (requestSize_ > 0)
	{
		int realAlloc = 0;
		buffer_ = static_cast<char*>(allocator_->DynamicPop(requestSize_, realAlloc));
		realSize_ = realAlloc;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
PooledMemoryChunk::PooledMemoryChunk(PooledMemoryChunk&& _other) noexcept
: allocator_(jc::Move(_other.allocator_))
, buffer_(_other.buffer_)
, requestSize_(_other.requestSize_)
, realSize_(_other.realSize_)
{
	_other.buffer_ = nullptr;
	_other.requestSize_ = 0;
	_other.realSize_ = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
PooledMemoryChunk& PooledMemoryChunk::operator=(PooledMemoryChunk&& _other) noexcept
{
	if (this == &_other)
	{
		return *this;
	}

	Release();

	allocator_ = jc::Move(_other.allocator_);
	buffer_ = _other.buffer_;
	requestSize_ = _other.requestSize_;
	realSize_ = _other.realSize_;

	_other.buffer_ = nullptr;
	_other.requestSize_ = 0;
	_other.realSize_ = 0;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PooledMemoryChunk::Reset(const MemoryPoolAbstractPtr& _allocator, int _requestSize)
{
	Release();

	allocator_ = _allocator;
	requestSize_ = _requestSize;
	realSize_ = 0;
	buffer_ = nullptr;

	if (allocator_ != nullptr && requestSize_ > 0)
	{
		int realAlloc = 0;
		buffer_ = static_cast<char*>(allocator_->DynamicPop(requestSize_, realAlloc));
		realSize_ = realAlloc;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
void PooledMemoryChunk::Release()
{
	if (allocator_ != nullptr && buffer_)
	{
		allocator_->DynamicPush(buffer_, requestSize_);
	}

	buffer_ = nullptr;
	requestSize_ = 0;
	realSize_ = 0;
}
