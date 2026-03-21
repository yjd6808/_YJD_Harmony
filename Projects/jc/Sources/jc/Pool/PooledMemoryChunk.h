/*
 * 작성자: 윤정도
 * 생성일: 2/7/2023 2:38:40 PM
 * =====================
 *
 */


#pragma once

#include "jc/Pool/MemoryPoolAbstract.h"

NS_JC_BEGIN

class PooledMemoryChunk
{
public:
	PooledMemoryChunk() = default;
	PooledMemoryChunk(const MemoryPoolAbstractPtr& _allocator, int _requestSize);

	// 복사 금지
	PooledMemoryChunk(const PooledMemoryChunk&) = delete;
	PooledMemoryChunk& operator=(const PooledMemoryChunk&) = delete;

	// 이동 허용
	PooledMemoryChunk(PooledMemoryChunk&& _other) noexcept;
	PooledMemoryChunk& operator=(PooledMemoryChunk&& _other) noexcept;

	~PooledMemoryChunk() { Release(); }

	void Reset(const MemoryPoolAbstractPtr& _allocator, int _requestSize);
	char* GetBuffer() const { return buffer_; }
	int GetRequestSize() const { return requestSize_; }
	int GetRealSize() const { return realSize_; }
	bool IsValid() const { return allocator_ != nullptr && buffer_ != nullptr; }
	MemoryPoolAbstractPtr GetAllocator() const { return allocator_; }

private:
	void Release();

	MemoryPoolAbstractPtr allocator_;
	char* buffer_ = nullptr;
	int requestSize_ = 0;
	int realSize_ = 0;
};

NS_END
