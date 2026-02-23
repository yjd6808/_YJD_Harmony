/*
 * 작성자: 윤정도
 * 생성일: 12/17/2022 1:57:58 AM
 * =====================
 * 메모리풀 릭 감시 중단 후 결과들이 저장될 구조체
 */


#pragma once

#include <jc/Pool/MemoryPoolDetail.h>

NS_JC_BEGIN

template <typename T>
class SharedPtr;
class MemoryPoolAbstract;
struct MemoryPoolCaptured
{
	MemoryPoolAbstract* pPool_{};
	_u64 totalLeaks_{};
	_s32 leakBlocks_[detail::MemoryBlockSizeMapSize_v]{};
};

using MemoryPoolCapturedPtr = SharedPtr<MemoryPoolCaptured>;

NS_END
