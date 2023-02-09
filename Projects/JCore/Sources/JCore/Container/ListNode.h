/*
	작성자 : 윤정도
	LinkedList에서 사용할 노드
*/

#pragma once

#pragma warning(push)
  #pragma warning (disable: 26495) // member variable 'Value' is not initialized

NS_JC_BEGIN

template <typename T, typename TAllocator>
struct ListNode final
{
	using TListNode = ListNode<T, TAllocator>;

	ListNode() {}
	~ListNode() noexcept {}

	template <typename... Args>
	void Construct(Args&&... args) {
		new (__builtin_addressof(Value)) T{ Forward<Args>(args)... };
	}

	void DeleteSelf() {
		Value.~T();

		TAllocator::template Deallocate<decltype(*this)>(this);
		// delete this;
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

NS_JC_END


#pragma warning(pop)