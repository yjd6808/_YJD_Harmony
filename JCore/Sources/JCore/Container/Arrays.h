/*
	�ۼ��� : ������
	�⺻ �迭�� �ٷ絵�� �����ִ� Ŭ����
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Core.h>
#include <JCore/TypeTraits.h>

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
	static void Sort(T* arr, int arrSize) {
		DebugAssert(arr == nullptr, "arr == nullptr");
		DebugAssert(arrSize < 1, "arrSize�� ������ 1�̻��̾���մϴ�.");
		QuickSort(arr, 0, arrSize - 1);
	}

	template <typename T>
	static void SortRange(T* arr, int startIdx, int endIdx) {
		DebugAssert(arr == nullptr, "arr == nullptr");
		DebugAssert(startIdx < 0 || startIdx > endIdx, "�ùٸ��� ���� �ε��� �����Դϴ�.");
		QuickSort(arr, startIdx, endIdx);
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
};

} // namespace JCore