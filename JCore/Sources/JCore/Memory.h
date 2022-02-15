/*
	작성자 : 윤정도
	메모리 조작을 도와주는 클래스입니다.
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Tuple.h>
#include <JCore/Exception.h>

namespace JCore {

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
		Byte* pValue = (Byte*)&value;

		for (int i = 0; i < srcCapacity; i++) {
			Copy(pSrc, kiDataTypeSize, pValue, kiDataTypeSize);
			pSrc += kiDataTypeSize;
		}
	}
	
	template <typename R>
	static R Allocate(const int size) {
		static_assert(IsPointerType_v<R>, "only cast to pointer type");
		try {
			return (R)::operator new(size);
		} catch (std::bad_alloc&) {
			throw InvalidOperationException("메모리 할당에 실패하였습니다.");
		}
	}

	static void Deallocate(void* ptr) {
		::operator delete(ptr);
	}

	template <typename T, typename... Args>
	static void PlacementAllocate(T& ref, Args&&... args) {
		::new (__builtin_addressof(ref)) T(Forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	static void PlacementAllocate(T* ref, Args&&... args) {
		::new (ref) T(Forward<Args>(args)...);
	}

	template <typename T>
	static void PlacementDeallocate(T& ref) {
		ref.~T();
	}

	template <typename T>
	static void PlacementDeallocate(const T* ref) {
		ref->~T();
	}
};

} // namespace JCore

