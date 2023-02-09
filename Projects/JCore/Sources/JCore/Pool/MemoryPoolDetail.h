/*
 * 작성자: 윤정도
 * 생성일: 12/15/2022 2:50:25 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/Container/Arrays.h>


NS_JC_BEGIN
NS_DETAIL_BEGIN

// 53바이트, 73바이트 뭐 이런식으로 정밀하게 할당해줄 수 없기때문에
// 53바이트를 요청하면 64바이트를
// 74바이트를 요청하면 128바이트를 할당해주는 식으로 처리
// 이후 내가 언급하는 블록은 "한번에 1, 2, 4, 8, 16 ... 8'388'608, 16'777'216에 해당하는 단위로 할당가능한 메모리양의 단위"를 의미한다.
inline constexpr int MemoryBlockSizeMap_v[] {
	1 << 0,  1 << 1,  1 << 2,  1 << 3,  1 << 4,
	1 << 5,  1 << 6,  1 << 7,  1 << 8,  1 << 9,
	1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14,
	1 << 15, 1 << 16, 1 << 17, 1 << 18, 1 << 19,
	1 << 20, 1 << 21, 1 << 22, 1 << 23, 1 << 24
};

inline char MemoryIndexSizeMap_v;

inline constexpr int MemoryBlockSizeMapSize_v = sizeof(MemoryBlockSizeMap_v) / sizeof(int);
inline constexpr int InvalidSlot_v = -1;

// 예를들어서
// 64바이트 -> 6으로 변환
//  6을    -> 64바이트로 변환 해주는 기능 수행
struct AllocationLengthMapConverter {
	template <Int32 Size, Int32 Index = 0>
	static constexpr int ToIndex() {
		static_assert(Index >= 0 && Index < MemoryBlockSizeMapSize_v, "... cannot find valid index [AllocationLengthMapConverter]");
		if constexpr (Size <= MemoryBlockSizeMap_v[Index])
			return Index;
		else
			return ToIndex<Size, Index + 1>();
	}


	template <Int32 Index>
	static constexpr int ToSize() {
		static_assert(Index >= 0 && Index < MemoryBlockSizeMapSize_v, "... cannot find valid size [AllocationLengthMapConverter]");
		return MemoryBlockSizeMap_v[Index];
	}

	static int ToIndex(Int32 size) {
		int iIndex = Arrays::LowerBound(Detail::MemoryBlockSizeMap_v, size);
		DebugAssertMsg(iIndex >= 0 && iIndex < MemoryBlockSizeMapSize_v, "전달한 Size로 할당가능한 사이즈에 맞는 풀이 없어요");
		return iIndex;
	}

	static int ToSize(Int32 index) {
		DebugAssertMsg(index >= 0 && index < MemoryBlockSizeMapSize_v, "전달한 Index에 해당하는 풀이 없어요");
		return Detail::MemoryBlockSizeMap_v[index];
	}

	// Set Bit 계산
	template <Int32 Size>
	static constexpr bool ValidateSize() {
		static_assert(Size > 0, "... Size must be greater than 0");
		int size = Size;
		int cnt = 0;
		while (true) {
			if ((size & 1) == 1) ++cnt;
			size >>= 1;
			if (size == 0) break;
		}

		if (cnt == 1) return true;
		return false;
	}

	// 런타임에는 성능이 중요하니 내장함수를 사용하자.
	static bool ValidateSize(Int32 size) {
		DebugAssertMsg(size > 0, "사이즈가 0보다는 무조건 커야돼요");
		return __popcnt(size) == 1;
	}
};

NS_DETAIL_END
NS_JC_END