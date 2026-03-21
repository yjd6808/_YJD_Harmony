/*
 *	작성자 : 윤정도
 *	메모리풀을 사용하지 않는 할당자
 */

#pragma once

#include "jc/Memory.h"

NS_JC_BEGIN

class CDefaultAllocator
{
public:
	// 할당 규칙
	// AllocateStatic<Data> => sizeof(Data)크기로 생성해서 return Data* 
	// AllocateStatic<Data*> => sizeof(Data)크기로 생성해서 return Data*
	// 포인터타입과 기본타입 모두 반환타입은 같음

	// Dynamic 반환시에는 실제로 반환된 크기도 같이 돌려준다.
	// 따라서 decay 시켜줘야함

	// 해제 규칙
	// DeallocateStatic<Data> => sizeof(Data)로 해제
	// DeallocateStatic<Data*> => sizeof(Data*)로 해제
	//  그래서 그냥 해제하면 됨.

	// 명시적으로 사이즈 요청해서 반환하는 2가지 기능을 구현할 것
	template <typename T>
	static auto AllocateStatic() // Static
	{
		using Type = NakedType_t<T>;
		auto pRet = Memory::Allocate<Type*>(sizeof(Type));
		return pRet;
	}

	template <typename T>
	static auto AllocateDynamic(int _size, int& _allocatedSize) // Dynamic
	{
		return Memory::Allocate<NakedType_t<T>*>(_size);
	}

	template <typename T, typename... Args>
	static auto AllocateInitStatic(Args&&... _args) // Static
	{
		auto pRet = Memory::Allocate<NakedType_t<T>*>(sizeof(NakedType_t<T>));
		Memory::PlacementNew(pRet, Forward<Args>(_args)...);
		return pRet;
	}

	template <typename T, typename... Args>
	static auto AllocateInitDynamic(int _size, int& _allocatedSize, Args&&... _args) // Dynamic
	{
		auto pRet = Memory::Allocate<NakedType_t<T>*>(_size);
		Memory::PlacementNew(pRet, Forward<Args>(_args)...);
		return pRet;
	}

	template <typename T>
	static void DeallocateStatic(void* _pDel)
	{
		Memory::Deallocate(_pDel);
	}

	static void DeallocateDynamic(void* _pDel, int _size)
	{
		Memory::Deallocate(_pDel);
	}
};

NS_END
