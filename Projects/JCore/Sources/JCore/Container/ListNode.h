/*
	작성자 : 윤정도
	LinkedList에서 사용할 노드
*/

#pragma once

#pragma warning(push)
  #pragma warning (disable: 26495) // member variable 'Value' is not initialized

NS_JC_BEGIN

template <typename T, typename TAllocator>
class ListNode final
{
public:
	using TListNode = ListNode<T, TAllocator>;

	ListNode()
	{
	}

	~ListNode() noexcept
	{
	}

	template <typename... Args>
	void Construct(Args&&... _args)
	{
		new (__builtin_addressof(value_)) T{ Forward<Args>(_args)... };
	}

	void DeleteSelf()
	{
		value_.~T();

		TAllocator::template DeallocateStatic<decltype(*this)>(this);
		// delete this;
	}

	union
	{
		T value_;    // Lazy Instantiation
	};

	TListNode* pNext_ = nullptr;
	TListNode* pPrevious_ = nullptr;
};


/*
template <typename T>
class CListNode<T*> {  };

template <typename T>
class CListNode<T&> {  };
*/

NS_JC_END


#pragma warning(pop)
