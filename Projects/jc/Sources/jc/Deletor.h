/*
	작성자 : 윤정도
	제거자 펑터
*/

#pragma once

#include <jc/Type.h>

NS_JC_BEGIN
// void 타입으로 삭제할 경우 소멸자가 호출되지 않는다
// 소멸자가 호출될 필요없는 경우에 사용하도록 하자.
// @참고 : https://stackoverflow.com/questions/941832/is-it-safe-to-delete-a-void-pointer

template <typename T>
struct Deletor
{
	Deletor() = default;

	void operator()(T* _ptr)
	{
		delete _ptr;
	}
};


template <typename T>
struct Deletor<T[]>
{
	Deletor() = default;

	void operator()(T* _ptr)
	{
		delete[] _ptr;
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
	static void DestroyObject(T* _ptr)
	{
		_ptr->~T();
	}

	void DeletePointer(T* _ptr)
	{
		operator delete(_ptr);
	}

	void DeleteBoth(T* _ptr)
	{
		_ptr->~T();
		operator delete(_ptr);
	}
};


template <typename T>
struct PlacementDeletorBase<T[]>
{
	static void DestroyObject(T* _ptr, _u32 _size)
	{
		for (_u32 i = 0; i < _size; i++)
		{
			_ptr[i].~T();
		}
	}

	void DeletePointer(T* _ptr)
	{
		operator delete[](_ptr);
	}

	void DeleteBoth(T* _ptr, _u32 _size)
	{
		for (_u32 i = 0; i < _size; i++)
		{
			_ptr[i].~T();
		}

		operator delete[](_ptr);
	}
};

template <typename T, _u32 ArraySize>
struct PlacementDeletorBase<T[ArraySize]>
{
	static void DestroyObject(T* _ptr)
	{
		for (_u32 i = 0; i < ArraySize; i++)
		{
			_ptr[i].~T();
		}
	}

	void DeletePointer(T* _ptr)
	{
		operator delete[](_ptr);
	}

	void DeleteBoth(T* _ptr)
	{
		for (_u32 i = 0; i < ArraySize; i++)
		{
			_ptr[i].~T();
		}

		operator delete[](_ptr);
	}
};


template <typename T, DeletorOption Option = DeletorOption::Both>
struct PlacementDeletor : PlacementDeletorBase<T>
{
	void operator()(T* _ptr)
	{
		if constexpr (Option == DeletorOption::Both)
		{
			this->DeleteBoth(_ptr);
		}
		else if constexpr (Option == DeletorOption::OnlyDestoryObject)
		{
			this->DestroyObject(_ptr);
		}
		else
		{
			this->DeletePointer(_ptr);
		}
	}
};

template <typename T, DeletorOption Option>
struct PlacementDeletor<T[], Option> : PlacementDeletorBase<T[]>
{
	void operator()(T* _ptr, _u32 _size)
	{
		if constexpr (Option == DeletorOption::Both)
		{
			this->DeleteBoth(_ptr, _size);
		}
		else if constexpr (Option == DeletorOption::OnlyDestoryObject)
		{
			this->DestroyObject(_ptr, _size);
		}
		else
		{
			this->DeletePointer(_ptr);
		}
	}
};

template <typename T, _u32 ArraySize, DeletorOption Option>
struct PlacementDeletor<T[ArraySize], Option> : PlacementDeletorBase<T[ArraySize]>
{
	void operator()(T* ptr)
	{
		if constexpr (Option == DeletorOption::Both)
		{
			this->DeleteBoth(ptr);
		}
		else if constexpr (Option == DeletorOption::OnlyDestoryObject)
		{
			this->DestroyObject(ptr);
		}
		else
		{
			this->DeletePointer(ptr);
		}
	}
};


NS_END
