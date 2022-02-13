/*
	�ۼ��� : ������
	�⺻ �迭�� �ٷ絵�� �����ִ� Ŭ����
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
	// ���̸� �ƴ� ���
	template <typename T, Int32U ArraySize>
	static void Sort(T(&arr)[ArraySize]) {
		QuickSort(arr, 0, ArraySize - 1);
	}

	// ���̸� �𸣴� ������Ÿ���� ���
	template <typename T>
	static void Sort(T* arr, const int arrSize) {
		ThrowIfArrayIsNull(arr);
		ThrowIfArraySizeIsInvalid(arrSize);
		QuickSort(arr, 0, arrSize - 1);
	}

	template <typename T>
	static void SortRange(T* arr, const int startIdx, const int endIdx) {
		ThrowIfArrayIsNull(arr);
		ThrowIRangeIsInvalid(startIdx, endIdx);
		QuickSort(arr, startIdx, endIdx);
	}


	/// <summary>
	/// ù ���Һ��� ���� Ž��
	/// </summary>
	template <typename T, Int32U ArraySize>
	static int LinearSearch(T(&arr)[ArraySize], const T& data) {
		return LinearSearch(arr, ArraySize, data);
	}


	// ���̸� �𸣴� ������ Ÿ���� ���
	template <typename T>
	static int LinearSearch(T* arr, const int arrSize, const T& data) {
		ThrowIfArrayIsNull(arr);
		ThrowIfArraySizeIsInvalid(arrSize);

		for (int i = 0; i < arrSize; i++) {
			if (arr[i] == data) {
				return i;
			}
		}

		return -1;
	}

	/// <summary>
	/// ������ ���Һ��� ���� Ž��
	/// </summary>
	template <typename T, Int32U ArraySize>
	static int LinearSearchReverse(T(&arr)[ArraySize], const T& data) {
		LinearSearchReverse(arr, ArraySize, data);
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
			int iComp = Comparator<T>()(arr[iMid], data);

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
	/// data�� ó������ ���۵Ǵ� ��ġ(�ε���)�� ��ȯ�Ѵ�.
	/// </summary>
	template <typename T, Int32U ArraySize>
	static int LowerBound(T(&arr)[ArraySize], const T& data) {
		return LowerBound(arr, ArraySize, data);
	}


	// @���� : https://12bme.tistory.com/120
	// LowerBound�� ������� ó���� �ſ� ��ٷӴ�.
	// �Ӹ��� ���ε��Ƽ� ������ ���ϰڴ�.
	template <typename T>
	static int LowerBound(T* arr, const int arrSize, const T& data) {
		ThrowIfArrayIsNull(arr);
		ThrowIfArraySizeIsInvalid(arrSize);

		return std::lower_bound(arr, arr + arrSize, data) - arr;
	}


	/// <summary>
	/// data ���� ū ���� �߿��� ���� ���� ���� ��ġ(�ε���)�� ��ȯ�Ѵ�.
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

	// @���� : https://www.youtube.com/watch?v=PgBzjlCcFvc&t=45s&ab_channel=GeeksforGeeks
	// �ҽ��ڵ�� ���� �ʰ� �ִϸ��̼Ǹ� ���� ¥�� �ڵ��Դϴ�.
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

private: // Throws
	template <typename T>
	static void ThrowIfArrayIsNull(T* arr) {
		if (arr == nullptr) {
			throw InvalidArgumentException("arr == nullptr");
		}
	}

	static void ThrowIfArraySizeIsInvalid(const int arrSize) {
		if (arrSize < 1) {
			throw InvalidArgumentException("arrSize�� ������ 1�̻��̾���մϴ�.");
		}
	}

	static void ThrowIRangeIsInvalid(const int startIdx, const int endIdx) {
		if (startIdx < 0 || startIdx > endIdx) {
			throw InvalidArgumentException("�ùٸ��� ���� �ε��� �����Դϴ�.");
		}
	}
};

} // namespace JCore