/*
	작성자 : 윤정도
	제거자 펑터
*/

#pragma once

namespace JCore {

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
	void DestroyObject(T* ptr) {
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
	void DestroyObject(T* ptr, Int32U size) {
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
	void DestroyObject(T* ptr) {
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