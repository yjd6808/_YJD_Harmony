#pragma once

/*
	작성자 : 윤정도
	
*/

#include <JCore/TypeTraits.h>

namespace JCore {

template <typename T>
class SharedPointer final
{
public:
	template <typename... Args)
	explicit SharedPointer(Args&&... args) {
		if (sizeof...(args) == 0) {
			m_pPointer = new T(Forward<Args>(args)...);
		}
	}
	virtual ~SharedPointer() {
	}
public:


private:
	T* m_pPointer;
};

} // namespace JCore