/*
	�ۼ��� : ������
*/

#pragma once

namespace JCore {

template <typename T>
struct ListNode
{
	using TListNode = ListNode<T>;

	template <typename... Args>
	explicit ListNode(Args&&... args) {
		Construct(Forward<Args>(args)...);
	}

	virtual ~ListNode() noexcept {
		Destroy();
	}

	template <typename... Args>
	void Construct(Args&&... args) {
		new (__builtin_addressof(Value)) T(Forward<Args>(args)...);
	}

	void Destroy() {
		Value.~T();
	}

	union { T Value; };	// Lazy Initialization�� ����
	TListNode* Next = nullptr;
	TListNode* Previous = nullptr;
};


} // namespace JCore