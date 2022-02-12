/*
	작성자 : 윤정도
	메모리 조작을 도와주는 클래스입니다.
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Tuple.h>

namespace JCore {

class Memory final
{
public:
	static void Copy(void* src, const int srcCapacity, const void* dst, const int dstCopySize);
	static void CopyUnsafe(void* src, const void* dst, const int dstCopySize);

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
		return (R)::operator new(size);
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

