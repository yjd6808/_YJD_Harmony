/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 2:36:50 PM
 * =====================
 *
 * 
 */


#pragma once

NS_JC_BEGIN

enum MemoryPoolAllocationAlgorithm
{
	eBinarySearch = 0x10,		// O(logn) / 2의 제곱 단위로만 할당가능
	eFullIndexing = 0x20,		// O(1)    / 2의 제곱 단위로만 할당가능
	eMemoryPoolAllocationAlgorithmMax = eFullIndexing,
	eMemoryPoolAllocationAlgorithmMask = 0xf0


	// eFixedSize		// O(?) / 자유롭게 할당가능
};

	NS_DETAIL_BEGIN
	template <MemoryPoolAllocationAlgorithm Algorithm>
	inline constexpr bool IsValidMemoryPoolAllocationAlgorithm = Algorithm >= eBinarySearch && Algorithm <= eMemoryPoolAllocationAlgorithmMax;
	NS_DETAIL_END
NS_JC_END