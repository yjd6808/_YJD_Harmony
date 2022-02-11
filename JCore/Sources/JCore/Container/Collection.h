/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/SmartPointer.h>
#include <JCore/PointerObserver.h>
#include <JCore/Memory.h>

#include <JCore/Container/Iterable.h>

#include <functional>

namespace JCore {

enum class ContainerType {
	ArrayQueue,
	ArrayStack,
	Vector,
	ListQueue,
	ListStack,
	LinkedList,
	HashMap,
	TreeMap
};

/*=====================================================================================
									콜렉션
							모든 컨테이너의 기본 인터페이스
=====================================================================================*/

template <typename T>
class Collection : public Iterable<T>
{
	using TCollection	= typename Collection<T>;
	using TIterator		= typename Iterator<T>;
public:
	Collection() : m_Owner(this, true) {}
	virtual ~Collection() noexcept { m_Owner.~VoidOwner(); }
public:
	void ForEach(std::function<void(T&)> fn) {
		auto it = this->Begin();
		while (it->HasValue()) {
			fn(it->Next());
		}
	}

protected:
	inline VoidOwner& GetOwner() const { return const_cast<VoidOwner&>(m_Owner); }
public:
	//inline SharedPointer<TCollection> ToCollection() = 0;

	bool IsEmpty() const { 
		return m_iSize == 0; 
	}

	int GetSize() const { 
		return m_iSize; 
	}
protected:
	int m_iSize = 0;
	union { VoidOwner m_Owner; };
};

} // namespace JCore