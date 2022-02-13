/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Memory.h>

#include <JCore/Container/Iterable.h>
#include <JCore/Container/Iterator.h>

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
	using TIterable		= typename Iterable<T>;
	using TCollection	= typename Collection<T>;
	using TEnumerator	= typename Enumerator<T>;
public:
	Collection() : TIterable(), m_Owner(this, true) {}
	virtual ~Collection() noexcept { m_Owner.~VoidOwner(); }
public:
	void ForEach(std::function<void(T&)> fn) {
		TEnumerator it = this->Begin();
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

	int Size() const { 
		return m_iSize; 
	}
protected:
	int m_iSize = 0;
	union { VoidOwner m_Owner; };
};

} // namespace JCore