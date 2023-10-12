/*
 *	작성자 : 윤정도
 *	메모리풀을 사용하지 않는 할당자
 */

#pragma once

#include <JCore/Memory.h>

NS_JC_BEGIN

class DefaultAllocator
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
	static auto AllocateStatic() {	// Static
		using Ty = NakedType_t<T>;
		return Memory::Allocate<Ty*>(sizeof(Ty));
	}

	template <typename T>
    static auto AllocateDynamic(int size, int& allocatedSize) {	// Dynamic
		return Memory::Allocate<NakedType_t<T>*>(size);
	}

	template <typename T, typename... Args>
	static auto AllocateInitStatic(Args&&... args) {	// Static
		auto pRet = Memory::Allocate<NakedType_t<T>*>(sizeof(NakedType_t<T>));
		Memory::PlacementNew(pRet, Forward<Args>(args)...);
		return pRet;
	}

	template <typename T, typename... Args>	
	static auto AllocateInitDynamic(int size, int& allocatedSize, Args&&... args) {	// Dynamic
		auto pRet = Memory::Allocate<NakedType_t<T>*>(size);
		Memory::PlacementNew(pRet, Forward<Args>(args)...);
		return pRet;
	}

	template <typename T>
    static void DeallocateStatic(void* del) {
		Memory::Deallocate(del);
    }

	static void DeallocateDynamic(void* del, int size) {
		Memory::Deallocate(del);
	}


};

NS_JC_END