/*
	작성자 : 윤정도
	기본 배열을 다루도록 도와주는 클래스
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Core.h>
#include <JCore/TypeTraits.h>
#include <JCore/Comparator.h>
#include <JCore/Exception.h>

namespace JCore {

struct Arrays final
{
	// 길이를 아는 경우
	template <typename T, Int32U ArraySize>
	static void Sort(T(&arr)[ArraySize]) {
		QuickSort(arr, 0, ArraySize - 1, NaturalOrder{});
	}

	// 길이를 아는 경우
	template <typename T, Int32U ArraySize, typename Predicate>
	static void Sort(T(&arr)[ArraySize], Predicate predicate) {
		QuickSort(arr, 0, ArraySize - 1, predicate);
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T>
	static void Sort(T* arr, const int arrSize) {
		ThrowIfArrayIsNull(arr);
		ThrowIfArraySizeIsInvalid(arrSize);
		QuickSort(arr, 0, arrSize - 1, NaturalOrder{});
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T, typename Predicate>
	static void Sort(T* arr, const int arrSize, Predicate predicate) {
		ThrowIfArrayIsNull(arr);
		ThrowIfArraySizeIsInvalid(arrSize);
		QuickSort(arr, 0, arrSize - 1, predicate);
	}

	template <typename T>
	static void SortRange(T* arr, const int startIdx, const int endIdx) {
		ThrowIfArrayIsNull(arr);
		ThrowIRangeIsInvalid(startIdx, endIdx);
		QuickSort(arr, startIdx, endIdx, NaturalOrder{});
	}

	template <typename T, typename Predicate>
	static void SortRange(T* arr, const int startIdx, const int endIdx, Predicate predicate) {
		ThrowIfArrayIsNull(arr);
		ThrowIRangeIsInvalid(startIdx, endIdx);
		QuickSort(arr, startIdx, endIdx, predicate);
	}

	
	/// <summary>
	/// 첫 원소부터 선형 탐색
	/// </summary>
	template <typename T, Int32U ArraySize>
	static int LinearSearch(T(&arr)[ArraySize], const T& data) {
		return LinearSearch(arr, ArraySize, data);
	}


	// 길이를 모르는 포인터 타입인 경우
	template <typename T>
	static int LinearSearch(T* arr, const int arrSize, const T& data) {
		ThrowIfArrayIsNull(arr);

		for (int i = 0; i < arrSize; i++) {
			if (arr[i] == data) {
				return i;
			}
		}

		return -1;
	}

	/// <summary>
	/// 마지막 원소부터 선형 탐색
	/// </summary>
	template <typename T, Int32U ArraySize>
	static int LinearSearchReverse(T(&arr)[ArraySize], const T& data) {
		return LinearSearchReverse(arr, ArraySize, data);
	}


	template <typename T>
	static int LinearSearchReverse(T* arr, const int arrSize, const T& data) {
		ThrowIfArrayIsNull(arr);
		ThrowIfArraySizeIsInvalid(arrSize);


		for (int i = arrSize - 1; i >= 0; i--) {
			if (arr[i] == data) {
				return i;
			}
		}

		return -1;
	}


	template <typename T, Int32U ArraySize>
	static int BinarySearch(T(&arr)[ArraySize], const T& data) {
		return BinarySearch(arr, ArraySize, data);
	}

	template <typename T>
	static int BinarySearch(T* arr, const int arrSize, const T& data) {
		ThrowIfArrayIsNull(arr);
		ThrowIfArraySizeIsInvalid(arrSize);

		int iStart = 0;
		int iEnd = arrSize - 1;

		while (iStart <= iEnd) {
			int iMid = (iStart + iEnd) / 2;
			const int iComp = Comparator<T>()(arr[iMid], data);

			if (arr[iMid] == data) {
				return iMid;
			}

			if (iComp == -1) {
				iStart = iMid + 1;
				continue;
			}
			iEnd = iMid - 1;
		}

		return -1;
	}

	/// <summary>
	/// data가 처음으로 시작되는 위치(인덱스)를 반환한다.
	/// </summary>
	template <typename T, Int32U ArraySize>
	static int LowerBound(T(&arr)[ArraySize], const T& data) {
		return LowerBound(arr, ArraySize, data);
	}


	// @참고 : https://12bme.tistory.com/120
	// LowerBound는 경계조건 처리가 매우 까다롭다.
	// 머리가 핑핑돌아서 구현을 못하겠다.
	template <typename T>
	static int LowerBound(T* arr, const int arrSize, const T& data) {
		ThrowIfArrayIsNull(arr);
		ThrowIfArraySizeIsInvalid(arrSize);

		return std::lower_bound(arr, arr + arrSize, data) - arr;
	}


	/// <summary>
	/// data 보다 큰 값들 중에서 가장 작은 값의 위치(인덱스)를 반환한다.
	/// </summary>
	template <typename T, Int32U ArraySize>
	static int UpperBound(T(&arr)[ArraySize], const T& data) {
		return UpperBound(arr, ArraySize, data);
	}
	

	template <typename T>
	static int UpperBound(T* arr, const int arrSize, const T& data) {
		ThrowIfArrayIsNull(arr);
		ThrowIfArraySizeIsInvalid(arrSize);

		return std::upper_bound(arr, arr + arrSize, data) - arr;
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
	template <typename T, typename Prdeicate>
	static void QuickSort(T* arr, int start, int end, Prdeicate predicate) {
		if (start >= end) {
			return;
		}

		int j = start - 1;
		int i = start;
		int pivot = end;

		while (i < pivot) {
			if (predicate(arr[i], arr[pivot])) {
				j++;
				Swap(arr, j, i);
			}
			i++;
		}
		j++;
		Swap(arr, j, pivot);
		pivot = j;
		
		QuickSort(arr, start, pivot - 1, predicate);
		QuickSort(arr, pivot + 1, end, predicate);
	}

private: // Throws
	template <typename T>
	static void ThrowIfArrayIsNull(T* arr) {
		if (arr == nullptr) {
			throw InvalidArgumentException("arr == nullptr");
		}
	}

	static void ThrowIfArraySizeIsInvalid(const int arrSize) {
		if (arrSize < 1) {
			throw InvalidArgumentException("arrSize는 무조건 1이상이어야합니다.");
		}
	}

	static void ThrowIRangeIsInvalid(const int startIdx, const int endIdx) {
		if (startIdx < 0 || startIdx > endIdx) {
			throw InvalidArgumentException("올바르지 않은 인덱스 범위입니다.");
		}
	}
};

} // namespace JCore