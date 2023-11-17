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

NS_JC_BEGIN

NS_DETAIL_BEGIN
constexpr int InvalidIndex_v = -1;
NS_DETAIL_END


struct Arrays final
{
	template <typename T, Int32U Size>
	static constexpr int Size(T(&)[Size]) {
		return Size;
	}

	// 길이를 아는 경우
	template <typename T, Int32U Size>
	static void Sort(T(&arr)[Size]) {
		QuickSort(arr, 0, Size - 1, NaturalOrder{});
	}

	// 길이를 아는 경우
	template <typename T, Int32U Size, typename TPredicate>
	static void Sort(T(&arr)[Size], TPredicate&& predicate) {
		QuickSort(arr, 0, Size - 1, Forward<TPredicate>(predicate));
	}

	// 길이를 아는 경우
	template <typename T, Int32U Size>
	static void InsertionSort(T(&arr)[Size]) {
		InsertionSortImpl(arr, Size, NaturalOrder{});
	}

	// 길이를 아는 경우
	template <typename T, Int32U Size, typename TPredicate>
	static void InsertionSort(T(&arr)[Size], TPredicate&& predicate) {
		InsertionSortImpl(arr, Size, Forward<TPredicate>(predicate));
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T>
	static void InsertionSort(T* arr, const int arrSize) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");
		InsertionSortImpl(arr, arrSize, NaturalOrder{});
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T, typename TPredicate>
	static void InsertionSort(T* arr, const int arrSize, TPredicate&& predicate) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");
		InsertionSortImpl(arr, arrSize, Forward<TPredicate>(predicate));
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T>
	static void Sort(T* arr, const int arrSize) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");
		QuickSort(arr, 0, arrSize - 1, NaturalOrder{});
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T, typename TPredicate>
	static void Sort(T* arr, const int arrSize, TPredicate&& predicate) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");
		QuickSort(arr, 0, arrSize - 1, Forward<TPredicate>(predicate));
	}

	template <typename T>
	static void SortRange(T* arr, const int startIdx, const int endIdx) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(startIdx >= 0 && startIdx <= endIdx, "올바르지 않은 인덱스 범위입니다.");
		QuickSort(arr, startIdx, endIdx, NaturalOrder{});
	}

	template <typename T, typename TPredicate>
	static void SortRange(T* arr, const int startIdx, const int endIdx, TPredicate&& predicate) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(startIdx >= 0 && startIdx <= endIdx, "올바르지 않은 인덱스 범위입니다.");
		QuickSort(arr, startIdx, endIdx, Forward<TPredicate>(predicate));
	}

	// 길이를 아는 경우
	template <typename T, Int32U Size>
	static void HeapSort(T(&arr)[Size]) {
		HeapSortImpl(arr, Size, NaturalOrder{});
	}

	// 길이를 아는 경우
	template <typename T, Int32U Size, typename TPredicate>
	static void HeapSort(T(&arr)[Size], TPredicate&& predicate) {
		HeapSortImpl(arr, Size, Forward<TPredicate>(predicate));
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T>
	static void HeapSort(T* arr, const int arrSize) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");
		HeapSortImpl(arr, arrSize, NaturalOrder{});
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T, typename TPredicate>
	static void HeapSort(T* arr, const int arrSize, TPredicate&& predicate) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");
		HeapSortImpl(arr, arrSize, 0, arrSize - 1, Forward<TPredicate>(predicate));
	}

	template <typename T>
	static void SortHeapRange(T* arr, const int startIdx, const int endIdx) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(startIdx >= 0 && startIdx <= endIdx, "올바르지 않은 인덱스 범위입니다.");
		HeapSortImpl(arr + startIdx, endIdx - startIdx + 1, NaturalOrder{});
	}

	template <typename T, typename TPredicate>
	static void SortHeapRange(T* arr, const int startIdx, const int endIdx, TPredicate&& predicate) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(startIdx >= 0 && startIdx <= endIdx, "올바르지 않은 인덱스 범위입니다.");
		HeapSortImpl(arr + startIdx, endIdx - startIdx + 1, Forward<TPredicate>(predicate));
	}




	/// <summary>
	/// 첫 원소부터 선형 탐색
	/// </summary>
	template <typename T, Int32U Size>
	static int LinearSearch(T(&arr)[Size], const T& data) {
		return LinearSearch(arr, Size, data);
	}


	// 길이를 모르는 포인터 타입인 경우
	template <typename T>
	static int LinearSearch(T* arr, const int arrSize, const T& data) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");

		for (int i = 0; i < arrSize; i++) {
			if (arr[i] == data) {
				return i;
			}
		}

		return Detail::InvalidIndex_v;
	}

	/// <summary>
	/// 마지막 원소부터 선형 탐색
	/// </summary>
	template <typename T, Int32U Size>
	static int LinearSearchReverse(T(&arr)[Size], const T& data) {
		return LinearSearchReverse(arr, Size, data);
	}


	template <typename T>
	static int LinearSearchReverse(T* arr, const int arrSize, const T& data) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");


		for (int i = arrSize - 1; i >= 0; i--) {
			if (arr[i] == data) {
				return i;
			}
		}

		return Detail::InvalidIndex_v;
	}


	template <typename T, Int32U Size>
	static int BinarySearch(T(&arr)[Size], const T& data) {
		return BinarySearch(arr, Size, data);
	}

	template <typename T>
	static int BinarySearch(T* arr, const int arrSize, const T& data) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");

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

		return Detail::InvalidIndex_v;
	}

	/// <summary>
	/// data가 처음으로 시작되는 위치(인덱스)를 반환한다.
	/// </summary>
	template <typename T, Int32U Size>
	static int LowerBound(const T(&arr)[Size], const T& data) {
		return LowerBound(arr, Size, data);
	}

	//	TPredicate은 T와 TVal을 비교해서 참이면 우측으로 탐색
	//                               거짓이면 좌측으로 탐색한다.
	//  이는 std::lower_bound 코드를보면 알 수 있다.
	/*  사용예시
		struct A { int a; int b; };
		A models[1001];

		// a는 1000 ~ 0 으로 초기화
		// b는 0 ~ 1000 으로 초기화
		for (int i = 0; i <= 1000; ++i) {
			models[i].a = 1000 - i;
			models[i].b = i;
		}


		// b값중 300을 찾고 싶다.
		// b는 오름차순 정렬되어있다.
		// model.a가 200이고, val이 300이 들어왔다.
		// 현재 탐색중인 곳인 model.a가 val보다 좌측에 있는 상황이다.
		// 따라서 300 근처의 값이 있는 우측을 탐색하기 위해서는 이 결과가 참이 되야한다.

		int idx = Arrays::LowerBound(&models[0], 1001, 300, [](const A& model, const int& val) {
			return model.b < val;
		});	// idx = 300이 들어감


		// a값중 300을 찾고 싶다.
		// a는 내림차순 정렬되어있다.
		// model.a가 500이고, val이 300이 들어왔다.
		// 현재 탐색중인 곳인 model.a가 val보다 좌측에 있는 상황이다.
		// 따라서 300 근처의 값이 있는 우측을 탐색하기 위해서는 이 결과가 참이 되야한다.

		int idx = Arrays::LowerBound(&models[0], 1001, 300, [](const A& model, const int& val) {
			return model.a > val;	// val은 계속 300이 들어옴
		});
	 */

	template <typename T, typename TVal, typename TPredicate>
	static int LowerBound(const T* arr, const int arrSize, TVal&& data, TPredicate&& predicate) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");

		return std::lower_bound(arr, arr + arrSize, Forward<TVal>(data), Forward<TPredicate>(predicate)) - arr;
	}

	// @참고 : https://12bme.tistory.com/120
	// LowerBound는 경계조건 처리가 매우 까다롭다.
	// 머리가 핑핑돌아서 구현을 못하겠다.
	template <typename T>
	static int LowerBound(const T* arr, const int arrSize, const T& data) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");

		return int(std::lower_bound(arr, arr + arrSize, data) - arr);
	}


	/// <summary>
	/// data 보다 큰 값들 중에서 가장 작은 값의 위치(인덱스)를 반환한다.
	/// </summary>
	template <typename T, Int32U Size>
	static int UpperBound(T(&arr)[Size], const T& data) {
		return UpperBound(arr, Size, data);
	}


	template <typename T>
	static int UpperBound(T* arr, const int arrSize, const T& data) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");

		return std::upper_bound(arr, arr + arrSize, data) - arr;
	}

	template <typename T, Int32U Size>
	static bool AllEqual(T(&arr)[Size]) {
		return AllEqual(arr, Size);
	}

	template <typename T>
	static bool AllEqual(T* arr, const int size) {
		DebugAssert(size > 0);

		T& elem = arr[0];
		for (int i = 1; i < size; ++i) {
			if (elem != arr[i]) {
				return false;
			}
		}
		return true;
	}

	template <typename T, Int32U Size, typename TVal>
	static bool AllEqual(T(&arr)[Size], const TVal& v) {
		return AllEqual(arr, Size, v);
	}


	template <typename T, typename TVal>
	static bool AllEqual(T* arr, const int size, const TVal& v) {
		DebugAssert(size > 0);

		for (int i = 0; i < size; ++i) {
			if (arr[i] != v) {
				return false;
			}
		}
		return true;
	}

	template <typename T, Int32U Size, typename TAction>
	static void ForEach(T(&arr)[Size], TAction&& action) {
		ForEach(Move(arr), Size, Move(action));
	}

	template <typename T, typename TAction>
	static void ForEach(T* arr, const int size, TAction&& action) {
		for (int i = 0; i < size; ++i) {
			action(arr[i]);
		}
	}

	template <typename T, Int32U Size>
	static void ForEachDelete(T(&arr)[Size]) {
		if constexpr (IsPointerType_v<T>) {
			for (int i = 0; i < Size; ++i) {
				JCORE_DELETE_SAFE(arr[i]);
			}
		}
	}

	template <typename T>
	static void ForEachDelete(T* arr, const int size) {
		if (arr == nullptr) return;
		if constexpr (IsPointerType_v<T>) {
			for (int i = 0; i < size; ++i) {
				JCORE_DELETE_SAFE(arr[i]);
			}
		}
	}

	template <typename T, typename TPredicate>
	static int FindIf(const T* arr, const int size, TPredicate&& predicate) {
		for (int i = 0; i < size; ++i)
			if (predicate(arr[i]))
				return i;

		return Detail::InvalidIndex_v;
	}

	template <typename T, Int32U Size, typename TPredicate>
	static int FindIf(const T(&arr)[Size], TPredicate&& predicate) {
		return FindIf(arr, Size, Forward<TPredicate>(predicate));
	}

	template <typename T, Int32U Size, typename TVal>
	static void Fill(T(&arr)[Size], TVal&& value) {
		for (int i = 0; i < Size; ++i) arr[i] = Forward<TVal>(value);
	}

	template <typename T, typename TVal>
	static void Fill(T* arr, int size, TVal&& value) {
		for (int i = 0; i < size; ++i) arr[i] = Forward<TVal>(value);
	}

	template <typename T, Int32U Size>
	static void Copy(T(&dst)[Size], const T(&src)[Size]) {
		for (int i = 0; i < Size; ++i)
			dst[i] = src[i];
	}

	template <typename T>
	static void CopyUnsafe(T* dst, const T* src, int count) {
		for (int i = 0; i < count; ++i)
			dst[i] = src[i];
	}

	template <typename T>
	static void Copy(T* dst, int capacity, const T* src, int count) {
		for (int i = 0; i < count && i < capacity; ++i)
			dst[i] = src[i];
	}

	template <typename T>
	static void Swap(T* arr, int l, int r) {
		T tmp = Move(arr[r]);
		arr[r] = Move(arr[l]);
		arr[l] = Move(tmp);
	}

	template <typename T>
	static void SwapCheck(T* arr, int size, int l, int r) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(l <= r && l >= 0 && l < size && r >= 0 && r < size, "올바르지 않은 인덱스 범위입니다.");

		Swap(arr, l, r);
	}

	template <typename T, Int32U Size>
	static void MakeHeap(T(&arr)[Size]) {
		MakeHeapRange(arr, 0, Size - 1, NaturalOrder{}); // call [2]
	}

	template <typename T>
	static void MakeHeap(T* arr, int arrSize) {
		MakeHeapRange(arr, 0, arrSize - 1, NaturalOrder{});	// call [2]
	}

	template <typename T, typename TPredicate>
	static void MakeHeap(T* arr, int arrSize, TPredicate&& predicate) {
		MakeHeapRange(arr, 0, arrSize - 1, Forward<TPredicate>(predicate));	// call [2]
	}

	// [2]
	template <typename T, typename TPredicate>
	static void MakeHeapRange(T* arr, int start, int end, TPredicate&& predicate) {
		MakeHeapSiftDown(arr + start, end - start + 1, Forward<TPredicate>(predicate));
	}

	template <typename T, typename TPredicate>
	static void MakeHeapSiftDown(T* arr, int arrSize, TPredicate&& predicate) {
		for (int i = arrSize / 2 - 1; i >= 0; --i) {
			HeapifySiftDown(arr, arrSize, i, Forward<TPredicate>(predicate));
		}
	}


	// 안씀 SiftDown이랑 성능 비교할려고 구현 해놓은거
	template <typename T, typename TPredicate>
	static void MakeHeapSiftUp(T* arr, int arrSize, TPredicate&& predicate) {
		for (int i = 0; i < arrSize; ++i) {
			HeapifySiftUp(arr, i, Forward<TPredicate>(predicate));
		}
	}

	template <typename T, typename TPredicate>
	static void HeapifySiftDown(T* arr, int arrSize, int i, TPredicate&& predicate) {
		int iCur = i;			// 우선순위가 더 낮아서 교체되어야하는 노드인덱스

		for (;;) {
			int iPrev = iCur;
			int iLeft = 2 * iCur + 1;
			int iRight = 2 * iCur + 2;


			// 3개중 제일 우선순위가 낮은 노드가 부모가 되어야함.
			if (iLeft < arrSize && !predicate(arr[iLeft], arr[iCur])) {
				iCur = iLeft;
			}

			if (iRight < arrSize && !predicate(arr[iRight], arr[iCur])) {
				iCur = iRight;
			}

			// 변경사항이 없다면
			if (iCur == iPrev) {
				return;
			}

			Swap(arr, iPrev, iCur);
		}
	}



	template <typename T, typename TPredicate>
	static void HeapifySiftUp(T* arr, int idx, TPredicate&& predicate) {
		int iCur = idx;

		for (;;) {
			int iPrev = iCur;
			int iParent = (iCur - 1) / 2;

			// 부모 노드가 우선순위가 더 낮다면
			if (iParent >= 0 && !predicate(arr[iCur], arr[iParent])) {
				iCur = iParent;
			}

			if (iCur == iPrev) {
				return;
			}

			Swap(arr, iPrev, iCur);
		}
	}

	template <typename T, Int32U Size>
	static void PushHeap(T(&arr)[Size]) {
		HeapifySiftUp(arr, Size, NaturalOrder{});
	}

	template <typename T>
	static void PushHeap(T* arr, int arrSize) {
		HeapifySiftUp(arr, arrSize, NaturalOrder{});
	}

	template <typename T, typename TPredicate>
	static void PushHeap(T* arr, int arrSize, TPredicate&& predicate) {
		HeapifySiftUp(arr, arrSize, Forward<TPredicate>(predicate));
	}

	template <typename T>
	static void PushHeapRange(T* arr, int start, int end) {
		HeapifySiftUp(arr + start, end - start + 1, NaturalOrder{});
	}

	template <typename T, typename TPredicate>
	static void PushHeapRange(T* arr, int start, int end, TPredicate&& predicate) {
		HeapifySiftUp(arr + start, end - start + 1, Forward<TPredicate>(predicate));
	}

	template <typename T, Int32U Size>
	static void PopHeap(T(&arr)[Size]) {
		Swap(arr, 0, Size - 1);
		HeapifySiftDown(arr, Size - 1, 0, NaturalOrder{});
	}

	template <typename T>
	static void PopHeap(T* arr, int arrSize) {
		Swap(arr, 0, arrSize - 1);
		HeapifySiftDown(arr, arrSize - 1, 0, NaturalOrder{});
	}

	template <typename T, typename TPredicate>
	static void PopHeap(T* arr, int arrSize, TPredicate&& predicate) {
		Swap(arr, 0, arrSize - 1);
		HeapifySiftDown(arr, arrSize - 1, 0, Forward<TPredicate>(predicate));
	}

	template <typename T>
	static void PopHeapRange(T* arr, int start, int end) {
		Swap(arr, start, end);
		HeapifySiftDown(arr + start, end - start, 0, NaturalOrder{});
	}

	template <typename T, typename TPredicate>
	static void PopHeapRange(T* arr, int start, int end, TPredicate&& predicate) {
		Swap(arr, start, end);
		HeapifySiftDown(arr + start, end - start, 0, Forward<TPredicate>(predicate));
	}
private:
	// @참고 : https://www.youtube.com/watch?v=PgBzjlCcFvc&t=45s&ab_channel=GeeksforGeeks
	// 영상에서 동작하는데로 만들기
	template <typename T, typename TPredicate>
	static void QuickSort(T* arr, int start, int end, TPredicate&& predicate) {
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

	template <typename T, typename TPredicate>
	static void HeapSortImpl(T* arr, const int arrSize, TPredicate&& predicate) {
		MakeHeap(arr, arrSize, Forward<TPredicate>(predicate));

		// size = 마지막원소를 제외한 힘 크기
		for (int size = arrSize - 1; size > 0; --size) {
			// 마지막 원소를 제외
			Swap(arr, 0, size);
			HeapifySiftDown(arr, size, 0, Forward<TPredicate>(predicate));
		}
	}



	template <typename T, typename TPredicate>
	static void InsertionSortImpl(T* arr, int size, TPredicate&& predicate) {
		for (int i = 1; i < size; ++i) {
			int find = i;
			for (int j = i - 1; j >= 0; --j) {

				if (predicate(arr[find], arr[j])) {
					T temp = Move(arr[j]);
					arr[j] = Move(arr[find]);
					arr[find] = Move(temp);
					find = j;
				} else {
					// 위치를 찾은 경우 바로 반환되므로 최선의 경우(대부분 정렬된 상태인 경우)에는
					// 대부분 j반복문 진입하자마자 break로 나가버림
					break;
				}
			}
		}
	}
	
};

NS_JC_END