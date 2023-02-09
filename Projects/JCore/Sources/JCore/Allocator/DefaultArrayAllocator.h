/*
 * 작성자: 윤정도
 * 생성일: 12/14/2022 12:53:24 PM
 * =====================
 * 시스템 배열관련 컨테이너 전용
 */


#pragma once


#include <JCore/Pool/BinarySearchMemoryPool.h>

NS_JC_BEGIN

inline BinarySearchMemoryPool ArrayAllocatorPool_v
{
	{
		/* 테스트 완료, 봉인
		{ 8, 0 },
		{ 16, 64 },
		{ 32, 128 },
		{ 64, 128 },
		{ 128, 128 },
		{ 256, 64 },
		{ 512, 64 },
		{ 1024, 32 },
		{ 2048, 32 },
		{ 4096, 16 },
		{ 8192, 16 },
		{ 8192 * 2, 8 },
		{ 8192 * 4, 8 }
		*/
	}
};

class DefaultArrayAllocator
{
public:
	// 명시적으로 사이즈 요청해서 반환하는 2가지 기능을 구현할 것
	template <typename T>
	static auto Allocate() {	// Static
		return (RemovePointer_t<T>*)ArrayAllocatorPool_v.StaticPop<sizeof(T)>();
	}

	template <typename T = void*>	// 명시하지 않을 경우 void* 반환
	static auto Allocate(int requestSize, int& realAllocatedSize) {	// Dynamic
		return (RemovePointer_t<T>*)ArrayAllocatorPool_v.DynamicPop(requestSize, realAllocatedSize);
	}

	template <typename T, typename... Args>
	static auto AllocateInit(Args&&... args) {	// Static
		auto pRet = (RemovePointer_t<T>*)ArrayAllocatorPool_v.StaticPop<sizeof(T)>();
		Memory::PlacementNew(pRet, Forward<Args>(args)...);
		return pRet;
	}

	template <typename T = void*, typename... Args>	// 명시하지 않을 경우 void* 반환
	static auto AllocateInit(int requestSize, int& realAllocatedSize, Args&&... args) {	// Dynamic
		auto pRet = (RemovePointer_t<T>*)ArrayAllocatorPool_v.DynamicPop(requestSize, realAllocatedSize);
		Memory::PlacementNew(pRet, Forward<Args>(args)...);
		return pRet;
	}

	template <typename T>
	static void Deallocate(void* del) {
		ArrayAllocatorPool_v.StaticPush<sizeof(T)>(del);
	}

	static void Deallocate(void* del, int size) {
		ArrayAllocatorPool_v.DynamicPush(del, size);
	}
};

NS_JC_END

