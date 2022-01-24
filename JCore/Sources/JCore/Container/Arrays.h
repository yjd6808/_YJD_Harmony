/*
	작성자 : 윤정도
	기본 배열을 다루도록 도와주는 클래스
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Core.h>
#include <JCore/TypeTraits.h>

namespace JCore {

struct Arrays final
{
	// 길이를 아는 경우
	template <typename T, Int32U ArraySize>
	static void Sort(T(&arr)[ArraySize]) {
		QuickSort(arr, 0, ArraySize - 1);
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T>
	static void Sort(T* arr, int arrSize) {
		DebugAssert(arr == nullptr, "arr == nullptr");
		DebugAssert(arrSize < 1, "arrSize는 무조건 1이상이어야합니다.");
		QuickSort(arr, 0, arrSize - 1);
	}

	template <typename T>
	static void SortRange(T* arr, int startIdx, int endIdx) {
		DebugAssert(arr == nullptr, "arr == nullptr");
		DebugAssert(startIdx < 0 || startIdx > endIdx, "올바르지 않은 인덱스 범위입니다.");
		QuickSort(arr, startIdx, endIdx);
	}
private:
	template <typename T>
	static void Swap(T* arr, int l, int r) {
		T tmp = Move(arr[r]);
		arr[r] = Move(arr[l]);
		arr[l] = Move(tmp);
	}

	// @참고 : https://www.youtube.com/watch?v=PgBzjlCcFvc&t=45s&ab_channel=GeeksforGeeks
	// 소스코드는 보지 않고 애니메이션만 보고 짜본 코드입니다.
	template <typename T>
	static void QuickSort(T* arr, int start, int end) {
		if (start >= end) {
			return;
		}

		int j = start - 1;
		int i = start;
		int pivot = end;

		while (i < pivot) {
			if (arr[i] < arr[pivot]) {
				j++;
				Swap(arr, j, i);
			}
			i++;
		}
		j++;
		Swap(arr, j, pivot);
		pivot = j;
		
		QuickSort(arr, start, pivot - 1);
		QuickSort(arr, pivot + 1, end);
	}
};

} // namespace JCore