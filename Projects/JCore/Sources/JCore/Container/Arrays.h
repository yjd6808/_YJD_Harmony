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
	template <typename T, Int32U ArraySize>
	static constexpr int Size(T(&)[ArraySize]) {
		return ArraySize;
	}

	// 길이를 아는 경우
	template <typename T, Int32U ArraySize>
	static void Sort(T(&arr)[ArraySize]) {
		QuickSort(arr, 0, ArraySize - 1, NaturalOrder{});
	}

	// 길이를 아는 경우
	template <typename T, Int32U ArraySize, typename TPredicate>
	static void Sort(T(&arr)[ArraySize], TPredicate&& predicate) {
		QuickSort(arr, 0, ArraySize - 1, Move(predicate));
	}

	// 길이를 아는 경우
	template <typename T, Int32U ArraySize>
	static void SortInsertion(T(&arr)[ArraySize]) {
		SortInsertion(arr, ArraySize, NaturalOrder{});
	}

	// 길이를 아는 경우
	template <typename T, Int32U ArraySize, typename TPredicate>
	static void SortInsertion(T(&arr)[ArraySize], TPredicate&& predicate) {
		SortInsertion(arr, ArraySize, Move(predicate));
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T>
	static void SortInsertion(T* arr, const int arrSize) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");
		InsertionSort(arr, arrSize, NaturalOrder{});
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T, typename TPredicate>
	static void SortInsertion(T* arr, const int arrSize, TPredicate&& predicate) {
		DebugAssertMsg(arr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");
		InsertionSort(arr, arrSize, Move(predicate));
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
		QuickSort(arr, 0, arrSize - 1, Move(predicate));
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
		QuickSort(arr, startIdx, endIdx, Move(predicate));
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
	template <typename T, Int32U ArraySize>
	static int LinearSearchReverse(T(&arr)[ArraySize], const T& data) {
		return LinearSearchReverse(arr, ArraySize, data);
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


	template <typename T, Int32U ArraySize>
	static int BinarySearch(T(&arr)[ArraySize], const T& data) {
		return BinarySearch(arr, ArraySize, data);
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
	template <typename T, Int32U ArraySize>
	static int LowerBound(const T(&arr)[ArraySize], const T& data) {
		return LowerBound(arr, ArraySize, data);
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
	template <typename T, Int32U ArraySize>
	static int UpperBound(T(&arr)[ArraySize], const T& data) {
		return UpperBound(arr, ArraySize, data);
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

	template <typename T, typename TPredicate>
	static int FindIf(const T* arr, const int size, TPredicate&& predicate) {
		for (int i = 0; i < size; ++i)
			if (predicate(arr[i]))
				return i;

		return Detail::InvalidIndex_v;
	}

	template <typename T, Int32U Size, typename TPredicate>
	static int FindIf(const T(&arr)[Size], TPredicate&& predicate) {
		return FindIf(arr, Size, Move(predicate));
	}

	template <typename T, Int32U Size>
	static void Fill(T(&arr)[Size], T&& value) {
		for (int i = 0; i < Size; ++i) arr[i] = Forward<T>(value);
	}

	template <typename T, Int32U Size>
	static void Fill(T(&arr)[Size], const T& value) {
		for (int i = 0; i < Size; ++i) arr[i] = value;
	}


	template <typename T>
	static void Fill(T* arr, int size, T&& value) {
		for (int i = 0; i < size; ++i) arr[i] = Forward<T>(value);
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

private:

	// @참고 : https://www.youtube.com/watch?v=PgBzjlCcFvc&t=45s&ab_channel=GeeksforGeeks
	// 영상에서 동작하는데로 만들기
	template <typename T, typename Prdeicate>
	static void QuickSort(T* arr, int start, int end, Prdeicate&& predicate) {
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

	template <typename T, typename Prdeicate>
	static void InsertionSort(T* arr, int size, Prdeicate predicate) {
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