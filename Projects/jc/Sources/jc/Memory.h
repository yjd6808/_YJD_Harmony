/*
	작성자 : 윤정도
	메모리 조작을 도와주는 클래스입니다.
*/

#pragma once

#include <jc/Type.h>
#include <jc/TypeTraits.h>
#include <jc/Exception.h>
#include <jc/Debug/New.h>

NS_JC_BEGIN
class Memory final
{
public:
	// 시작 바이트부터 복사 시작
	static void Copy(void* _dst, int _dstCapacityByte, const void* _src, int _srcCopyByte);
	static void CopyUnsafe(void* _dst, const void* _src, int _srcCopyByte);

	// 마지막 바이트부터 복사 시작
	static void CopyReverse(void* _dst, int _dstCapacityByte, const void* _src, int _srcCopyByte);
	static void CopyUnsafeReverse(void* _dst, const void* _src, int _srcCopyByte);


	static void Set(void* _src, int _srcCapacity, Byte _value);

	// 자료형 T 크기 단위로 복사를 진행합니다.
	template <typename T>
	static void Set(T* _src, const int _srcCapacity, const T _value)
	{
		const int kiDataTypeSize = sizeof(T);

		Byte* pSrc = (Byte*)_src;
		const Byte* pValue = (Byte*)&_value;

		for (int i = 0; i < _srcCapacity; i++)
		{
			Copy(pSrc, kiDataTypeSize, pValue, kiDataTypeSize);
			pSrc += kiDataTypeSize;
		}
	}

	// 메모리풀 구현전 임시로 사용할 메모리 할당 해제 함수
	template <typename T>
	static T Allocate(const Int32U _size)
	{
		static_assert(IsPointerType_v<T>, "only cast to pointer type");

		try { return (T)dbg_operator_new(_size); }
		catch (std::bad_alloc&) { throw InvalidOperationException("메모리 할당에 실패하였습니다."); }
	}

	static void Deallocate(void* _ptr)
	{
		//jc_assert_msg(ptr != nullptr, "널 포인터를 해제하려고 하고 있어요");

		::operator delete(_ptr);
	}


	/*
	 * data_  t1(0, 0)
	 * data_* t2 = new data_(1, 1)
	 *
	 * PlacementAllocate(t1, 10, 10)
	 *	 T	= data_
	 *   T&	= data_&
	 *	   => (2)를 수행해줘야함
	 * PlacementAllocate(t2, 20, 20)
	 *   T	= data_*
	 *	 T& = data_*&
	 *	   == (1)을 수행해줘야함
	 *
	 * Igore은 PlacementNew 기능이 필요없는 대상을 상대로 적용
	 *  => CVector<data_> 는 PlacementNew 기능이 필요하다.
	 *  => CVector<data_*>는 PlacementNew 기능이 필요없다.
	 *
	 *	그리고 인터페이스 클래스의 생성자/소멸자를 통해 직접적으로 생성 불가능한경우 이 함수를 호출하지 않도록 할 수 있다.
	 *	  => private 혹은 protected로 보호된 경우
	 *	  => 순수가상함수가 구현되어서 인스턴스를 생성할 수 없는 상태인 경우
	 *	
	 */
	template <typename T, typename... Args>
	static void PlacementNew(T& _ref, Args&&... _args)
	{
		using GenType = NakedType_t<T>;

		if constexpr (IsConstructible_v<GenType, Args...>)
		{
			if constexpr (IsPointerType_v<T>)
				::new(_ref) GenType{Forward<Args>(_args)...}; // (1)
			else
				::new(AddressOf(_ref)) GenType(Forward<Args>(_args)...); // (2)		
		}
	}

	template <bool Ignore = false, typename T, typename... Args>
	static bool PlacementNewArray(T* _ref, int _size, Args&&... _args)
	{
		using GenType = NakedType_t<T>;

		if constexpr (Ignore)
			return false;

		for (int i = 0; i < _size; ++i)
		{
			PlacementNew(_ref[i], Forward<Args>(_args)...);
		}
		return true;
	}

	template <bool Ignore = false, typename T>
	static void PlacementDelete(T& _ref)
	{
		using DelType = NakedType_t<T>;

		if constexpr (Ignore)
			return;
		else if constexpr (IsPointerType_v<T>)
			_ref->~DelType();
		else
			_ref.~DelType();
	}

	template <bool Ignore = false, typename T>
	static void PlacementDeleteArray(T* _arr, int _size)
	{
		using DelType = NakedType_t<T>;

		if constexpr (Ignore)
			return;

		for (int i = 0; i < _size; ++i)
		{
			PlacementDelete(_arr[i]);
		}
	}
};

NS_JC_END
