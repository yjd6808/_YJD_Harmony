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
	constexpr int INVALID_INDEX = -1;
NS_DETAIL_END

struct Arrays final
{
	template <typename T, Int32U Size>
	static constexpr int Size(T (&_array)[Size])
	{
		return Size;
	}

	// 길이를 아는 경우
	template <typename T, Int32U Size>
	static void Sort(T (&_arr)[Size])
	{
		QuickSort(_arr, 0, Size - 1, NaturalOrder{});
	}

	// 길이를 아는 경우
	template <typename T, Int32U Size, typename TPredicate>
	static void Sort(T (&_arr)[Size], TPredicate&& _predicate)
	{
		QuickSort(_arr, 0, Size - 1, Forward<TPredicate>(_predicate));
	}

	// 길이를 아는 경우
	template <typename T, Int32U Size>
	static void InsertionSort(T (&_arr)[Size])
	{
		InsertionSortImpl(_arr, Size, NaturalOrder{});
	}

	// 길이를 아는 경우
	template <typename T, Int32U Size, typename TPredicate>
	static void InsertionSort(T (&_arr)[Size], TPredicate&& _predicate)
	{
		InsertionSortImpl(_arr, Size, Forward<TPredicate>(_predicate));
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T>
	static void InsertionSort(T* _pArr, const int _arrSize)
	{
		DebugAssertMsg(_pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");
		InsertionSortImpl(_pArr, _arrSize, NaturalOrder{});
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T, typename TPredicate>
	static void InsertionSort(T* _pArr, const int _arrSize, TPredicate&& _predicate)
	{
		DebugAssertMsg(_pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");
		InsertionSortImpl(_pArr, _arrSize, Forward<TPredicate>(_predicate));
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T>
	static void Sort(T* _pArr, const int _arrSize)
	{
		DebugAssertMsg(_pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");
		QuickSort(_pArr, 0, _arrSize - 1, NaturalOrder{});
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T, typename TPredicate>
	static void Sort(T* _pArr, const int _arrSize, TPredicate&& _predicate)
	{
		DebugAssertMsg(_pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");
		QuickSort(_pArr, 0, _arrSize - 1, Forward<TPredicate>(_predicate));
	}

	template <typename T>
	static void SortRange(T* _pArr, const int _startIndex, const int _endIndex)
	{
		DebugAssertMsg(_pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_startIndex >= 0 && _startIndex <= _endIndex, "올바르지 않은 인덱스 범위입니다.");
		QuickSort(_pArr, _startIndex, _endIndex, NaturalOrder{});
	}

	template <typename T, typename TPredicate>
	static void SortRange(T* _pArr, const int _startIndex, const int _endIndex, TPredicate&& _predicate)
	{
		DebugAssertMsg(_pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_startIndex >= 0 && _startIndex <= _endIndex, "올바르지 않은 인덱스 범위입니다.");
		QuickSort(_pArr, _startIndex, _endIndex, Forward<TPredicate>(_predicate));
	}

	// 길이를 아는 경우
	template <typename T, Int32U Size>
	static void HeapSort(T (&_arr)[Size])
	{
		HeapSortImpl(_arr, Size, NaturalOrder{});
	}

	// 길이를 아는 경우
	template <typename T, Int32U Size, typename TPredicate>
	static void HeapSort(T (&_arr)[Size], TPredicate&& _predicate)
	{
		HeapSortImpl(_arr, Size, Forward<TPredicate>(_predicate));
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T>
	static void HeapSort(T* _pArr, const int _arrSize)
	{
		DebugAssertMsg(_pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");
		HeapSortImpl(_pArr, _arrSize, NaturalOrder{});
	}

	// 길이를 모르는 포인터타입인 경우
	template <typename T, typename TPredicate>
	static void HeapSort(T* _pArr, const int _arrSize, TPredicate&& _predicate)
	{
		DebugAssertMsg(_pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");
		HeapSortImpl(_pArr, _arrSize, 0, _arrSize - 1, Forward<TPredicate>(_predicate));
	}

	template <typename T>
	static void SortHeapRange(T* _pArr, const int _startIndex, const int _endIndex)
	{
		DebugAssertMsg(_pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_startIndex >= 0 && _startIndex <= _endIndex, "올바르지 않은 인덱스 범위입니다.");
		HeapSortImpl(_pArr + _startIndex, _endIndex - _startIndex + 1, NaturalOrder{});
	}

	template <typename T, typename TPredicate>
	static void SortHeapRange(T* _pArr, const int _startIndex, const int _endIndex, TPredicate&& _predicate)
	{
		DebugAssertMsg(_pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_startIndex >= 0 && _startIndex <= _endIndex, "올바르지 않은 인덱스 범위입니다.");
		HeapSortImpl(_pArr + _startIndex, _endIndex - _startIndex + 1, Forward<TPredicate>(_predicate));
	}

	/// <summary>
	/// 첫 원소부터 선형 탐색
	/// </summary>
	template <typename T, Int32U Size>
	static int LinearSearch(T (&_arr)[Size], const T& _data)
	{
		return LinearSearch(_arr, Size, _data);
	}

	// 길이를 모르는 포인터 타입인 경우
	template <typename T>
	static int LinearSearch(T* _pArr, const int _arrSize, const T& _data)
	{
		DebugAssertMsg(_pArr != nullptr, "배열이 null입니다.");

		for (int index = 0; index < _arrSize; ++index)
		{
			if (_pArr[index] == _data)
			{
				return index;
			}
		}

		return Detail::INVALID_INDEX;
	}

	/// <summary>
	/// 마지막 원소부터 선형 탐색
	/// </summary>
	template <typename T, Int32U Size>
	static int LinearSearchReverse(T (&_arr)[Size], const T& _data)
	{
		return LinearSearchReverse(_arr, Size, _data);
	}

	template <typename T>
	static int LinearSearchReverse(T* _pArr, const int _arrSize, const T& _data)
	{
		DebugAssertMsg(_pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");

		for (int index = _arrSize - 1; index >= 0; --index)
		{
			if (_pArr[index] == _data)
			{
				return index;
			}
		}

		return Detail::INVALID_INDEX;
	}

	template <typename T, Int32U Size>
	static int BinarySearch(T (&_arr)[Size], const T& _data)
	{
		return BinarySearch(_arr, Size, _data);
	}

	template <typename T>
	static int BinarySearch(T* _pArr, const int _arrSize, const T& _data)
	{
		DebugAssertMsg(_pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");

		int start = 0;
		int end = _arrSize - 1;

		while (start <= end)
		{
			int middle = (start + end) / 2;
			const int comparison = Comparator<T>()(_pArr[middle], _data);

			if (_pArr[middle] == _data)
			{
				return middle;
			}

			if (comparison == -1)
			{
				start = middle + 1;
				continue;
			}
			end = middle - 1;
		}

		return Detail::INVALID_INDEX;
	}

	/// <summary>
	/// data가 처음으로 시작되는 위치(인덱스)를 반환한다.
	/// </summary>
	template <typename T, Int32U Size>
	static int LowerBound(const T (&_arr)[Size], const T& _data)
	{
		return LowerBound(_arr, Size, _data);
	}

	//	TPredicate은 T와 TVal을 비교해서 참이면 우측으로 탐색
	//                               거짓이면 좌측으로 탐색한다.
	//  이는 std::lower_bound 코드를보면 알 수 있다.
	/*  사용예시
		struct A { int a; int b; };
		A models[1001];

		// a는 1000 ~ 0 으로 초기화
		// b는 0 ~ 1000 으로 초기화
		for (int index = 0; index <= 1000; ++index) {
			models[index].a = 1000 - index;
			models[index].b = index;
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

		idx = Arrays::LowerBound(&models[0], 1001, 300, [](const A& model, const int& val) {
			return model.a > val;	// val은 계속 300이 들어옴
		});
	 */

	template <typename T, typename TVal, typename TPredicate>
	static int LowerBound(const T* pArr, const int _arrSize, TVal&& _data, TPredicate&& _predicate)
	{
		DebugAssertMsg(pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");

		return static_cast<int>(std::lower_bound(pArr, pArr + _arrSize, Forward<TVal>(_data),
		                                         Forward<TPredicate>(_predicate)) - pArr);
	}

	// @참고 : https://12bme.tistory.com/120
	// LowerBound는 경계조건 처리가 매우 까다롭다.
	// 머리가 핑핑돌아서 구현을 못하겠다.
	template <typename T>
	static int LowerBound(const T* pArr, const int _arrSize, const T& _data)
	{
		DebugAssertMsg(pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");

		return static_cast<int>(std::lower_bound(pArr, pArr + _arrSize, _data) - pArr);
	}

	/// <summary>
	/// data 보다 큰 값들 중에서 가장 작은 값의 위치(인덱스)를 반환한다.
	/// </summary>
	template <typename T, Int32U Size>
	static int UpperBound(T (&_arr)[Size], const T& _data)
	{
		return UpperBound(_arr, Size, _data);
	}

	template <typename T>
	static int UpperBound(T* _pArr, const int _arrSize, const T& _data)
	{
		DebugAssertMsg(_pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_arrSize >= 1, "arrSize는 무조건 1이상이어야합니다.");

		return static_cast<int>(std::upper_bound(_pArr, _pArr + _arrSize, _data) - _pArr);
	}

	template <typename T, Int32U Size>
	static bool AllEqual(T (&_arr)[Size])
	{
		return AllEqual(_arr, Size);
	}

	template <typename T>
	static bool AllEqual(T* _pArr, const int _size)
	{
		DebugAssert(_size > 0);

		T& element = _pArr[0];
		for (int index = 1; index < _size; ++index)
		{
			if (element != _pArr[index])
			{
				return false;
			}
		}
		return true;
	}

	template <typename T, Int32U Size, typename TVal>
	static bool AllEqual(T (&_arr)[Size], const TVal& _value)
	{
		return AllEqual(_arr, Size, _value);
	}

	template <typename T, typename TVal>
	static bool AllEqual(T* _pArr, const int _size, const TVal& _value)
	{
		DebugAssert(_size > 0);

		for (int index = 0; index < _size; ++index)
		{
			if (_pArr[index] != _value)
			{
				return false;
			}
		}
		return true;
	}

	template <typename T, Int32U Size, typename TAction>
	static void ForEach(T (&_arr)[Size], TAction&& _action)
	{
		ForEach(Move(_arr), Size, Move(_action));
	}

	template <typename T, typename TAction>
	static void ForEach(T* _pArr, const int _size, TAction&& _action)
	{
		for (int index = 0; index < _size; ++index)
		{
			_action(_pArr[index]);
		}
	}

	template <typename T, Int32U Size>
	static void ForEachDelete(T (&_arr)[Size])
	{
		if constexpr (IsPointerType_v<T>)
		{
			for (int index = 0; index < Size; ++index)
			{
				JCORE_DELETE_SAFE(_arr[index]);
			}
		}
	}

	template <typename T>
	static void ForEachDelete(T* _pArr, const int _size)
	{
		if (_pArr == nullptr)
		{
			return;
		}

		if constexpr (IsPointerType_v<T>)
		{
			for (int index = 0; index < _size; ++index)
			{
				JCORE_DELETE_SAFE(_pArr[index]);
			}
		}
	}

	template <typename T, typename TPredicate>
	static int FindIf(const T* pArr, const int _size, TPredicate&& _predicate)
	{
		for (int index = 0; index < _size; ++index)
		{
			if (_predicate(pArr[index]))
			{
				return index;
			}
		}

		return Detail::INVALID_INDEX;
	}

	template <typename T, Int32U Size, typename TPredicate>
	static int FindIf(const T (&_arr)[Size], TPredicate&& _predicate)
	{
		return FindIf(_arr, Size, Forward<TPredicate>(_predicate));
	}

	template <typename T, Int32U Size, typename TVal>
	static void Fill(T (&_arr)[Size], TVal&& _value)
	{
		for (int index = 0; index < Size; ++index)
		{
			_arr[index] = Forward<TVal>(_value);
		}
	}

	template <typename T, typename TVal>
	static void Fill(T* _pArr, int _size, TVal&& _value)
	{
		for (int index = 0; index < _size; ++index)
		{
			_pArr[index] = Forward<TVal>(_value);
		}
	}

	template <typename T, Int32U Size>
	static void Copy(T (&_destination)[Size], const T (&_source)[Size])
	{
		for (int index = 0; index < Size; ++index)
		{
			_destination[index] = _source[index];
		}
	}

	template <typename T>
	static void CopyUnsafe(T* pDestination, const T* pSource, int _count)
	{
		for (int index = 0; index < _count; ++index)
		{
			pDestination[index] = pSource[index];
		}
	}

	template <typename T>
	static void Copy(T* pDestination, int _capacity, const T* pSource, int _count)
	{
		for (int index = 0; index < _count && index < _capacity; ++index)
		{
			pDestination[index] = pSource[index];
		}
	}

	template <typename T>
	static void Swap(T* pArr, int _left, int _right)
	{
		T temp = Move(pArr[_right]);
		pArr[_right] = Move(pArr[_left]);
		pArr[_left] = Move(temp);
	}

	template <typename T>
	static void SwapCheck(T* pArr, int _size, int _left, int _right)
	{
		DebugAssertMsg(pArr != nullptr, "배열이 null입니다.");
		DebugAssertMsg(_left <= _right && _left >= 0 && _left < _size && _right >= 0 && _right < _size,
		               "올바르지 않은 인덱스 범위입니다.");

		Swap(pArr, _left, _right);
	}

	template <typename T, Int32U Size>
	static void MakeHeap(T (&_arr)[Size])
	{
		MakeHeapRange(_arr, 0, Size - 1, NaturalOrder{}); // call [2]
	}

	template <typename T>
	static void MakeHeap(T* _pArr, int _arrSize)
	{
		MakeHeapRange(_pArr, 0, _arrSize - 1, NaturalOrder{}); // call [2]
	}

	template <typename T, typename TPredicate>
	static void MakeHeap(T* _pArr, int _arrSize, TPredicate&& _predicate)
	{
		MakeHeapRange(_pArr, 0, _arrSize - 1, Forward<TPredicate>(_predicate)); // call [2]
	}

	// [2]
	template <typename T, typename TPredicate>
	static void MakeHeapRange(T* _pArr, int _start, int _end, TPredicate&& _predicate)
	{
		MakeHeapSiftDown(_pArr + _start, _end - _start + 1, Forward<TPredicate>(_predicate));
	}

	template <typename T, typename TPredicate>
	static void MakeHeapSiftDown(T* _pArr, int _arrSize, TPredicate&& _predicate)
	{
		for (int index = _arrSize / 2 - 1; index >= 0; --index)
		{
			HeapifySiftDown(_pArr, _arrSize, index, Forward<TPredicate>(_predicate));
		}
	}

	// 안씀 SiftDown이랑 성능 비교할려고 구현 해놓은거
	template <typename T, typename TPredicate>
	static void MakeHeapSiftUp(T* _pArr, int _arrSize, TPredicate&& _predicate)
	{
		for (int index = 0; index < _arrSize; ++index)
		{
			HeapifySiftUp(_pArr, index, Forward<TPredicate>(_predicate));
		}
	}

	template <typename T, typename TPredicate>
	static void HeapifySiftDown(T* _pArr, int _arrSize, int _index, TPredicate&& _predicate)
	{
		int currentIndex = _index;

		for (;;)
		{
			int previousIndex = currentIndex;
			int leftIndex = 2 * currentIndex + 1;
			int rightIndex = 2 * currentIndex + 2;

			// 3개중 제일 우선순위가 낮은 노드가 부모가 되어야함.
			if (leftIndex < _arrSize && !_predicate(_pArr[leftIndex], _pArr[currentIndex]))
			{
				currentIndex = leftIndex;
			}

			if (rightIndex < _arrSize && !_predicate(_pArr[rightIndex], _pArr[currentIndex]))
			{
				currentIndex = rightIndex;
			}

			// 변경사항이 없다면
			if (currentIndex == previousIndex)
			{
				return;
			}

			Swap(_pArr, previousIndex, currentIndex);
		}
	}

	template <typename T, typename TPredicate>
	static void HeapifySiftUp(T* _pArr, int _index, TPredicate&& _predicate)
	{
		int currentIndex = _index;

		for (;;)
		{
			int previousIndex = currentIndex;
			int parentIndex = (currentIndex - 1) / 2;

			// 부모 노드가 우선순위가 더 낮다면
			if (parentIndex >= 0 && !_predicate(_pArr[currentIndex], _pArr[parentIndex]))
			{
				currentIndex = parentIndex;
			}

			if (currentIndex == previousIndex)
			{
				return;
			}

			Swap(_pArr, previousIndex, currentIndex);
		}
	}

	template <typename T, Int32U Size>
	static void PushHeap(T (&_arr)[Size])
	{
		HeapifySiftUp(_arr, Size, NaturalOrder{});
	}

	template <typename T>
	static void PushHeap(T* _pArr, int _arrSize)
	{
		HeapifySiftUp(_pArr, _arrSize, NaturalOrder{});
	}

	template <typename T, typename TPredicate>
	static void PushHeap(T* _pArr, int _arrSize, TPredicate&& _predicate)
	{
		HeapifySiftUp(_pArr, _arrSize, Forward<TPredicate>(_predicate));
	}

	template <typename T>
	static void PushHeapRange(T* _pArr, int _start, int _end)
	{
		HeapifySiftUp(_pArr + _start, _end - _start + 1, NaturalOrder{});
	}

	template <typename T, typename TPredicate>
	static void PushHeapRange(T* _pArr, int _start, int _end, TPredicate&& _predicate)
	{
		HeapifySiftUp(_pArr + _start, _end - _start + 1, Forward<TPredicate>(_predicate));
	}

	template <typename T, Int32U Size>
	static void PopHeap(T (&_arr)[Size])
	{
		Swap(_arr, 0, Size - 1);
		HeapifySiftDown(_arr, Size - 1, 0, NaturalOrder{});
	}

	template <typename T>
	static void PopHeap(T* _pArr, int _arrSize)
	{
		if (_arrSize <= 0)
		{
			DebugAssert(false);
			return;
		}

		Swap(_pArr, 0, _arrSize - 1);
		HeapifySiftDown(_pArr, _arrSize - 1, 0, NaturalOrder{});
	}

	template <typename T, typename TPredicate>
	static void PopHeap(T* _pArr, int _arrSize, TPredicate&& _predicate)
	{
		if (_arrSize <= 0)
		{
			DebugAssert(false);
			return;
		}

		Swap(_pArr, 0, _arrSize - 1);
		HeapifySiftDown(_pArr, _arrSize - 1, 0, Forward<TPredicate>(_predicate));
	}

	template <typename T>
	static void PopHeapRange(T* _pArr, int _start, int _end)
	{
		Swap(_pArr, _start, _end);
		HeapifySiftDown(_pArr + _start, _end - _start, 0, NaturalOrder{});
	}

	template <typename T, typename TPredicate>
	static void PopHeapRange(T* _pArr, int _start, int _end, TPredicate&& _predicate)
	{
		Swap(_pArr, _start, _end);
		HeapifySiftDown(_pArr + _start, _end - _start, 0, Forward<TPredicate>(_predicate));
	}

private:
	// @참고 : https://www.youtube.com/watch?v=PgBzjlCcFvc&t=45s&ab_channel=GeeksforGeeks
	// 영상에서 동작하는데로 만들기
	template <typename T, typename TPredicate>
	static void QuickSort(T* _pArr, int _start, int _end, TPredicate&& _predicate)
	{
		if (_start >= _end)
		{
			return;
		}

		int partitionIndex = _start - 1;
		int index = _start;
		int pivotIndex = _end;

		while (index < pivotIndex)
		{
			if (_predicate(_pArr[index], _pArr[pivotIndex]))
			{
				++partitionIndex;
				Swap(_pArr, partitionIndex, index);
			}
			++index;
		}
		++partitionIndex;
		Swap(_pArr, partitionIndex, pivotIndex);
		pivotIndex = partitionIndex;

		QuickSort(_pArr, _start, pivotIndex - 1, _predicate);
		QuickSort(_pArr, pivotIndex + 1, _end, _predicate);
	}

	template <typename T, typename TPredicate>
	static void HeapSortImpl(T* _pArr, const int _arrSize, TPredicate&& _predicate)
	{
		MakeHeap(_pArr, _arrSize, Forward<TPredicate>(_predicate));

		// size = 마지막원소를 제외한 힘 크기
		for (int size = _arrSize - 1; size > 0; --size)
		{
			// 마지막 원소를 제외
			Swap(_pArr, 0, size);
			HeapifySiftDown(_pArr, size, 0, Forward<TPredicate>(_predicate));
		}
	}

	template <typename T, typename TPredicate>
	static void InsertionSortImpl(T* _pArr, int _size, TPredicate&& _predicate)
	{
		for (int index = 1; index < _size; ++index)
		{
			int foundIndex = index;
			for (int compareIndex = index - 1; compareIndex >= 0; --compareIndex)
			{
				if (_predicate(_pArr[foundIndex], _pArr[compareIndex]))
				{
					T temp = Move(_pArr[compareIndex]);
					_pArr[compareIndex] = Move(_pArr[foundIndex]);
					_pArr[foundIndex] = Move(temp);
					foundIndex = compareIndex;
				}
				else
				{
					// 위치를 찾은 경우 바로 반환되므로 최선의 경우(대부분 정렬된 상태인 경우)에는
					// 대부분 compareIndex반복문 진입하자마자 break로 나가버림
					break;
				}
			}
		}
	}
};

using Arrays = Arrays;

NS_JC_END
