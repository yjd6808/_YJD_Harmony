/*
	작성자 : 윤정도
	할당자 만들기
*/

#pragma once

namespace JCore {

template <typename T>
struct DefaultAllocator
{
	template <typename... Args>
    static T* Allocate(Args&&... args) {
		return new T{ static_cast<Args&&>(args)... };
	}

    static void Deallocate(const T* del) {
        delete del;
    }
};

} // namespace JCore