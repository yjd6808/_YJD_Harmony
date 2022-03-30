/*
	�ۼ��� : ������
	������ ����
*/

#pragma once

#include <JCore/Type.h>

namespace JCore {

// void Ÿ������ ������ ��� �Ҹ��ڰ� ȣ����� �ʴ´�
// �Ҹ��ڰ� ȣ��� �ʿ���� ��쿡 ����ϵ��� ����.
// @���� : https://stackoverflow.com/questions/941832/is-it-safe-to-delete-a-void-pointer

struct VoidDeletor
{
	VoidDeletor() = default;

	void operator()(void * ptr) const {
		delete ptr;
	}
};

struct VoidDeletorSafe
{
	VoidDeletorSafe() = default;

	void operator()(void** ptr) const {
		delete *ptr;
		*ptr = nullptr;
	}
};

struct ArrayVoidDeletor
{
	ArrayVoidDeletor() = default;

	void operator()(void* ptr) const {
		delete[] ptr;
	}
};

struct ArrayVoidDeletorSafe
{
	ArrayVoidDeletorSafe() = default;

	void operator()(void** ptr) const {
		delete[] *ptr;
		*ptr = nullptr;
	}
};

template <typename T>
struct Deletor {
	Deletor() = default;

	void operator()(T* ptr) {
		delete ptr;
	}
};


template <typename T>
struct Deletor<T[]>
{
	Deletor() = default;

	void operator()(T* ptr) {
		delete[] ptr;
	}
};

enum class DeletorOption
{
	OnlyDestoryObject,
	OnlyDeletePointer,
	Both
};



template <typename T>
struct PlacementDeletorBase
{
	static void DestroyObject(T* ptr) {
		ptr->~T();
	}

	void DeletePointer(T* ptr) {
		operator delete(ptr);
	}

	void DeleteBoth(T* ptr) {
		ptr->~T();
		operator delete(ptr);
	}
};


template <typename T>
struct PlacementDeletorBase<T[]>
{
	static void DestroyObject(T* ptr, Int32U size) {
		for (Int32U i = 0; i < size; i++) {
			ptr[i].~T();
		}
	}

	void DeletePointer(T* ptr) {
		operator delete[](ptr);
	}

	void DeleteBoth(T* ptr, Int32U size) {
		for (Int32U i = 0; i < size; i++) {
			ptr[i].~T();
		}

		operator delete[](ptr);
	}

};

template <typename T, Int32U ArraySize>
struct PlacementDeletorBase<T[ArraySize]>
{
	static void DestroyObject(T* ptr) {
		for (Int32U i = 0; i < ArraySize; i++) {
			ptr[i].~T();
		}
	}

	void DeletePointer(T* ptr) {
		operator delete[](ptr);
	}

	void DeleteBoth(T* ptr) {
		for (Int32U i = 0; i < ArraySize; i++) {
			ptr[i].~T();
		}

		operator delete[](ptr);
	}
};


template <typename T, DeletorOption Option = DeletorOption::Both>
struct PlacementDeletor : PlacementDeletorBase<T>
{
	void operator()(T* ptr) {
		if constexpr (Option == DeletorOption::Both) {
			this->DeleteBoth(ptr);
		} else if constexpr (Option == DeletorOption::OnlyDestoryObject) {
			this->DestroyObject(ptr);
		} else {
			this->DeletePointer(ptr);
		}
	}
};

template <typename T, DeletorOption Option>
struct PlacementDeletor<T[], Option> : PlacementDeletorBase<T[]>
{
	void operator()(T* ptr, Int32U size) {
		if constexpr (Option == DeletorOption::Both) {
			this->DeleteBoth(ptr, size);
		} else if constexpr (Option == DeletorOption::OnlyDestoryObject) {
			this->DestroyObject(ptr, size);
		} else {
			this->DeletePointer(ptr);
		}
	}
};

template <typename T, Int32U ArraySize, DeletorOption Option>
struct PlacementDeletor<T[ArraySize], Option> : PlacementDeletorBase<T[ArraySize]>
{
	void operator()(T* ptr) {
		if constexpr (Option == DeletorOption::Both) {
			this->DeleteBoth(ptr);
		} else if constexpr (Option == DeletorOption::OnlyDestoryObject) {
			this->DestroyObject(ptr);
		} else {
			this->DeletePointer(ptr);
		}
	}
};




} // namespace JCore