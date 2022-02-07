/*
	작성자 : 윤정도
	할당자 만들기
*/

#pragma once

namespace JCore {

template <typename T>
struct Allocator
{
	template <typename... Args>
	T* operator new(Args&&... args) {
		return new T{ static_cast<Args&&>(args)... };
	}
};

} // namespace JCore