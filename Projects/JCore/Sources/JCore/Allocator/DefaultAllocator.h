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
	// Allocate<Data> => sizeof(Data)크기로 생성해서 return Data* 
	// Allocate<Data*> => sizeof(Data)크기로 생성해서 return Data*
	// 포인터타입과 기본타입 모두 반환타입은 같음

	// Dynamic 반환시에는 실제로 반환된 크기도 같이 돌려준다.
	// 따라서 decay 시켜줘야함

	// 해제 규칙
	// Deallocate<Data> => sizeof(Data)로 해제
	// Deallocate<Data*> => sizeof(Data*)로 해제
	//  그래서 그냥 해제하면 됨.

	// 명시적으로 사이즈 요청해서 반환하는 2가지 기능을 구현할 것
	template <typename T>
	static auto Allocate() {	// Static
		return Memory::Allocate<RemovePointer_t<T>*>(sizeof(T));
	}

	template <typename T = void*>	// 명시하지 않을 경우 void* 반환
    static auto Allocate(int size, int& allocatedSize) {	// Dynamic
		return Memory::Allocate<RemovePointer_t<T>*>(size);
	}

	template <typename T, typename... Args>
	static auto AllocateInit(Args&&... args) {	// Static
		auto pRet = Memory::Allocate<RemovePointer_t<T>*>(sizeof(T));
		Memory::PlacementNew(pRet, Forward<Args>(args)...);
		return pRet;
	}

	template <typename T = void*, typename... Args>	// 명시하지 않을 경우 void* 반환
	static auto AllocateInit(int size, int& allocatedSize, Args&&... args) {	// Dynamic
		auto pRet = Memory::Allocate<RemovePointer_t<T>*>(size);
		Memory::PlacementNew(pRet, Forward<Args>(args)...);
		return pRet;
	}

	template <typename T>
    static void Deallocate(void* del) {
		Memory::Deallocate(del);
    }

	static void Deallocate(void* del, int size) {
		Memory::Deallocate(del);
	}


};

NS_JC_END