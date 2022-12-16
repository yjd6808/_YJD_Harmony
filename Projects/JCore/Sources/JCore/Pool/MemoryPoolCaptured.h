/*
 * 작성자: 윤정도
 * 생성일: 12/17/2022 1:57:58 AM
 * =====================
 * 메모리풀 릭 감시 중단 후 결과들이 저장될 구조체
 */


#pragma once

#include <JCore/Pool/MemoryPoolDetail.h>

namespace JCore {

	class MemoryPoolAbstract;
	class MemoryPoolCaptured
	{
	public:
		MemoryPoolAbstract* Pool{};
		Int64U TotalLeaks;
		Int32 LeakBlocks[Detail::MemoryBlockSizeMapSize_v];
	};
} // namespace JCore

