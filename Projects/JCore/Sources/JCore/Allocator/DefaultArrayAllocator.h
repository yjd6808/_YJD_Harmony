/*
 * 작성자: 윤정도
 * 생성일: 12/14/2022 12:53:24 PM
 * =====================
 * 시스템 배열관련 컨테이너 전용
 */


#pragma once

#include <JCore/System/JCoreSystem.h>

#define AssertMessage "헉! 메모리풀 초기화가 아직 이뤄지지 않았어요. 전역 변수의 생성순서를 체크해주세요! 혹은 JCoreSystem.cpp에서 제대로 초기화 되는지 확인해주세요"

namespace JCore {

	struct DefaultArrayAllocator
	{
		// 명시적으로 사이즈 요청해서 반환하는 2가지 기능을 구현할 것
		template <typename T>
		static auto Allocate() {	// Static
			DebugAssertMessage(JCoreArrayAllocatorPool_v.GetPtr(), AssertMessage);
			return (RemovePointer_t<T>*)JCoreArrayAllocatorPool_v->StaticPop<sizeof(T)>();
		}

		template <typename T = void*>	// 명시하지 않을 경우 void* 반환
		static auto Allocate(int requestSize, int& realAllocatedSize) {	// Dynamic
			DebugAssertMessage(JCoreArrayAllocatorPool_v.GetPtr(), AssertMessage);
			return (RemovePointer_t<T>*)JCoreArrayAllocatorPool_v->DynamicPop(requestSize, realAllocatedSize);
		}

		template <typename T>
		static void Deallocate(void* del) {
			DebugAssertMessage(JCoreArrayAllocatorPool_v.GetPtr(), AssertMessage);
			JCoreArrayAllocatorPool_v->StaticPush<sizeof(T)>(del);
		}

		static void Deallocate(void* del, int size) {
			DebugAssertMessage(JCoreArrayAllocatorPool_v.GetPtr(), AssertMessage);
			JCoreArrayAllocatorPool_v->DynamicPush(del, size);
		}
	};

} // namespace JCore

#undef AssertMessage