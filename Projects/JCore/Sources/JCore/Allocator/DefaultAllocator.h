/*
	작성자 : 윤정도
	할당자 만들기
*/

#pragma once

#include <JCore/Memory.h>

namespace JCore {

template <typename T>
struct DefaultAllocator
{
    static T* Allocate(const Int32U size) {
		return Memory::Allocate<T*>(size);
	}

    static void Deallocate(T** del) {
		Memory::Deallocate(*del);
		*del = nullptr;
    }
};

} // namespace JCore