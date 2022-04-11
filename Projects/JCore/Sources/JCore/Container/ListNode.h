/*
	작성자 : 윤정도
*/

#pragma once

namespace JCore {

template <typename T>
struct ListNode
{
	using TListNode = ListNode<T>;

	ListNode() {}
	virtual ~ListNode() noexcept {}

	template <typename... Args>
	void Construct(Args&&... args) {
		new (__builtin_addressof(Value)) T{ Forward<Args>(args)... };
	}

	void DeleteSelf() const {
		Value.~T();
		delete this;
	}

	union { T Value; };	// Lazy Instantiation
	TListNode* Next = nullptr;
	TListNode* Previous = nullptr;
};


/*
template <typename T>
struct ListNode<T*> {  };

template <typename T>
struct ListNode<T&> {  };
*/

} // namespace JCore