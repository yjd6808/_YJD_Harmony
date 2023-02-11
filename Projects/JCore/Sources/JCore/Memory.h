/*
	작성자 : 윤정도
	메모리 조작을 도와주는 클래스입니다.
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Tuple.h>
#include <JCore/TypeTraits.h>
#include <JCore/Exception.h>
#include <JCore/Debug/New.h>
#include <JCore/Assert.h>

#include <stdio.h>

NS_JC_BEGIN

class Memory final
{
public:
	// 시작 바이트부터 복사 시작
	static void Copy(void* dst, const int dstCapacityByte, const void* src, const int srcCopyByte);
	static void CopyUnsafe(void* dst, const void* src, const int srcCopyByte);

	// 마지막 바이트부터 복사 시작
	static void CopyReverse(void* dst, const int dstCapacityByte, const void* src, const int srcCopyByte);
	static void CopyUnsafeReverse(void* dst, const void* src, const int srcCopyByte);

	

	static void Set(void* src, const int srcCapacity, const Byte value);

	// 자료형 T 크기 단위로 복사를 진행합니다.
	template <typename T>
	static void Set(T* src, const int srcCapacity, const T value) {
		const int kiDataTypeSize = sizeof(T);

		Byte* pSrc = (Byte*)src;
		const Byte* pValue = (Byte*)&value;

		for (int i = 0; i < srcCapacity; i++) {
			Copy(pSrc, kiDataTypeSize, pValue, kiDataTypeSize);
			pSrc += kiDataTypeSize;
		}
	}

	// 메모리풀 구현전 임시로 사용할 메모리 할당 해제 함수
	template <typename T>
	static T Allocate(const Int32U size) {
		static_assert(IsPointerType_v<T>, "only cast to pointer type");

		try { return (T)dbg_operator_new(size); }
		catch (std::bad_alloc&) { throw InvalidOperationException("메모리 할당에 실패하였습니다."); }
	}

	static void Deallocate(void* ptr) {
		//DebugAssertMsg(ptr != nullptr, "널 포인터를 해제하려고 하고 있어요");

		::operator delete(ptr);
	}


	/*
	 * Data  t1(0, 0)
	 * Data* t2 = new Data(1, 1)
	 *
	 * PlacementAllocate(t1, 10, 10)
	 *	 T	= Data
	 *   T&	= Data&
	 *	   => (2)를 수행해줘야함
	 * PlacementAllocate(t2, 20, 20)
	 *   T	= Data*
	 *	 T& = Data*&
	 *	   == (1)을 수행해줘야함
	 *
	 * Igore은 PlacementNew 기능이 필요없는 대상을 상대로 적용
	 *  => Vector<Data> 는 PlacementNew 기능이 필요하다.
	 *  => Vector<Data*>는 PlacementNew 기능이 필요없다.
	 *
	 *	그리고 인터페이스 클래스의 생성자/소멸자를 통해 직접적으로 생성 불가능한경우 이 함수를 호출하지 않도록 할 수 있다.
	 *	  => private 혹은 protected로 보호된 경우
	 *	  => 순수가상함수가 구현되어서 인스턴스를 생성할 수 없는 상태인 경우
	 *	
	 */
	template <bool Ignore = false, typename T, typename... Args >
	static void PlacementNew(T& ref, Args&&... args) {
		using GenType = NakedType_t<T>;

		if constexpr (Ignore)
			return;
		else if constexpr (IsPointerType_v<T>)								
			::new (ref) GenType{ Forward<Args>(args)... };	// (1)
		else															
			::new (AddressOf(ref)) GenType(Forward<Args>(args)...); // (2)
	}

	template <bool Ignore = false, typename T, typename... Args >
	static void PlacementNewArray(T* ref, int size, Args&&... args) {
		using GenType = NakedType_t<T>;

		if constexpr (Ignore)
			return;

		for (int i = 0; i < size; ++i) {
			PlacementNew(ref[i], Forward<Args>(args)...);
		}
	}

	template <bool Ignore = false, typename T>
	static void PlacementDelete(T& ref) {
		using DelType = NakedType_t<T>;

		if constexpr (Ignore)
			return;
		else if constexpr (IsPointerType_v<T>)
			ref->~DelType();
		else
			ref.~DelType();
	}

	template <bool Ignore = false, typename T>
	static void PlacementDeleteArray(T* arr, int size) {
		using DelType = NakedType_t<T>;

		if constexpr (Ignore)
			return;

		for (int i = 0; i < size; ++i) {
			PlacementDelete(arr[i]);
		}
	}

};

NS_JC_END

