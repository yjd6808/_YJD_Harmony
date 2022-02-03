/*
	작성자 : 윤정도
	스마트 포인터 만들어보기
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Exception.h>
#include <JCore/TypeTraits.h>

#ifndef DeleteSafe
#define DeleteSafe(x)			\
do {							\
	if (x) {					\
		delete x;				\
	}							\
	x = nullptr;				\
} while (0);					
#endif

namespace JCore {

// 전방 선언
template <typename> class WeakPointer;
template <typename> class SharedPointer;
template <typename> class UniquePointer;


template <typename T, typename... Args>
constexpr decltype(auto) MakeShared(Args&&... args) {
	return SharedPointer<T>::Create(Forward<Args>(args)...);
}

template <typename T, typename... Args>
constexpr decltype(auto) MakeUnique(Args&&... args) {
	return UniquePointer<T>::Create(Forward<Args>(args)...);
}

/*

MakeShared, MakeUnique 함수 안만들고
원래 생성자에 가변 템플릿 인자를 받을 수 있도록 할려고 했는데..
그러면 멤버변수로 쉐어드 포인터를 가지고 있는 경우 생성자에서 재귀 호출이 계속 발생해서 스택 오버플로가 발생한다.

template <typename T>
class SharedPointer
{
public:
	template <typename... Args>
	SharedPointer(Args&&... args) {
		m_Pointer = new T{ Forward<Args>(args)... };
		m_ControlBlock = new ControlBlock(1, 1);
	}
}

struct Test 
{
	SharedPoitner<Test> sp; // 무한 재귀호출 발생
}

*/

/*=====================================================================================
										유니크 포인터
 =====================================================================================*/

template <typename T>
class UniquePointer
{
private:
	using TUniquePointer = typename UniquePointer<T>;
private:
	UniquePointer(T* ptr) : m_Pointer(ptr) {}
public:
	UniquePointer() : m_Pointer(nullptr) {}
	UniquePointer(std::nullptr_t nulptr) : m_Pointer(nullptr) {}
	UniquePointer(const TUniquePointer& weak) = delete;
	~UniquePointer() { DeleteSafe(m_Pointer); }

	T& operator*() const {
		if (m_Pointer == nullptr) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}
		return *m_Pointer;
	}

	T* operator->() const {
		return m_Pointer;
	}

	T* Get() const {
		return m_Pointer;
	}

	template <typename AnyType>
	void operator=(AnyType&& other) = delete;
public:
	template <typename... Args>
	constexpr static TUniquePointer Create(Args&&... args) {
		return TUniquePointer{ new T{ Forward<Args>(args)... } };
	}
private:
	T* m_Pointer;
};








/*=====================================================================================
										쉐어드 포인터
 =====================================================================================*/
struct ControlBlock
{
	Int32 ReferenceCount;
	Int32 WeakCount;

	ControlBlock(Int32 refCount, Int32 weakCount) {
		ReferenceCount = refCount;
		WeakCount = weakCount;
	}
};

template <typename T>
class SharedPointer
{
private:
	using TSharedPointer = typename SharedPointer<T>;
	using TWeakPointer = typename WeakPointer<T>;
private:
	SharedPointer(T* pointer, ControlBlock* controlBlock) : m_Pointer(pointer), m_ControlBlock(controlBlock) {}
public:
	SharedPointer() {}
	SharedPointer(std::nullptr_t nulptr) {}

	SharedPointer(TWeakPointer& weak) {
		ChangeToWeak(weak);
	}

	SharedPointer(TWeakPointer&& weak) {
		MoveToWeak(weak);
	}

	SharedPointer(TSharedPointer& shared) {
		ChangeToShared(shared);
	}

	SharedPointer(TSharedPointer&& shared) {
		MoveToShared(shared);
	}

	~SharedPointer() {
		SubtractReferenceCount(*this);
	}

	T& operator*() const {
		if (!Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return *m_Pointer;
	}

	T* operator->() const {
		if (!Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return m_Pointer;
	}

	T* Get() const {
		if (!Exist()) {
			return nullptr;
		}

		return m_Pointer;
	}

	Int32U RefCount() const {
		if (m_ControlBlock == nullptr) {
			return 0;
		}

		return m_ControlBlock->ReferenceCount;
	}

	Int32U WeakCount() const {
		if (m_ControlBlock == nullptr) {
			return 0;
		}

		return m_ControlBlock->WeakCount;
	}

	bool Exist() const {
		if (m_ControlBlock == nullptr) {
			return false;
		}

		return m_ControlBlock->ReferenceCount > 0;
	}

	TSharedPointer& operator=(std::nullptr_t ptr) {
		MakeEmpty();
		return *this;
	}

	TSharedPointer& operator=(TSharedPointer& other) {
		ChangeToShared(other);
		return *this;
	}

	TSharedPointer& operator=(TSharedPointer&& other) {
		MoveToShared(other);
		return *this;
	}

	TSharedPointer& operator=(TWeakPointer& other) {
		ChangeToWeak(other);
		return *this;
	}

	TSharedPointer& operator=(TWeakPointer&& other) {
		MoveToWeak(other);
		return *this;
	}
public:
	template <typename... Args>
	constexpr static TSharedPointer Create(Args&&... args) {
		return TSharedPointer{ new T{ Forward<Args>(args)... }, new ControlBlock(1, 1) };
	}
private:
	void MakeEmpty() {
		SubtractReferenceCount(*this);
		m_Pointer = nullptr;
		m_ControlBlock = nullptr;
	}

	void ChangeToShared(TSharedPointer& shared) {
		SubtractReferenceCount(*this);
		m_Pointer = shared.m_Pointer;
		m_ControlBlock = shared.m_ControlBlock;
		AddReferenceCount(*this);
	}

	void ChangeToWeak(TWeakPointer& weak) {
		SubtractReferenceCount(*this);
		m_Pointer = weak.m_Pointer;
		m_ControlBlock = weak.m_ControlBlock;
		AddReferenceCount(*this);
	}

	void MoveToShared(TSharedPointer& shared) {
		SubtractReferenceCount(*this);
		m_Pointer = shared.m_Pointer;
		m_ControlBlock = shared.m_ControlBlock;

		shared.m_Pointer = nullptr;
		shared.m_ControlBlock = nullptr;
	}

	void MoveToWeak(TWeakPointer& weak) {
		SubtractReferenceCount(*this);
		m_Pointer = weak.m_Pointer;
		m_ControlBlock = weak.m_ControlBlock;

		weak.m_Pointer = nullptr;
		weak.m_ControlBlock = nullptr;
	}

	static void AddReferenceCount(TSharedPointer& shared) {
		shared.m_ControlBlock->ReferenceCount++;
		shared.m_ControlBlock->WeakCount++;
	}

	static void SubtractReferenceCount(TSharedPointer& shared) {
		if (shared.m_Pointer == nullptr || shared.m_ControlBlock == nullptr) {
			return;
		}

		if (shared.m_ControlBlock->ReferenceCount > 0)
			shared.m_ControlBlock->ReferenceCount--;

		if (shared.m_ControlBlock->WeakCount > 0)
			shared.m_ControlBlock->WeakCount--;

		if (shared.m_ControlBlock->ReferenceCount == 0)
			DeleteSafe(shared.m_Pointer);

		if (shared.m_ControlBlock->WeakCount == 0)
			DeleteSafe(shared.m_ControlBlock);
	}
protected:
	T* m_Pointer = nullptr;
	ControlBlock* m_ControlBlock = nullptr;

	friend class TWeakPointer;
};



/*=====================================================================================
										위크 포인터
 =====================================================================================*/
template <typename T>
class WeakPointer
{
private:
	using TSharedPointer	= typename SharedPointer<T>;
	using TWeakPointer		= typename WeakPointer<T>;
public:
	WeakPointer() {}
	WeakPointer(std::nullptr_t nulptr) {}
	WeakPointer(TWeakPointer& weak) {
		ChangeToWeak(weak);
	}

	WeakPointer(TWeakPointer&& weak) {
		MoveToWeak(weak);
	}

	WeakPointer(TSharedPointer& shared) {
		ChangeToShared(shared);
	}

	// 쉐어드 포인터의 이동은 막도록 하자.
	// 본체가 사라진 녀석을 참조해버릴 수 있으니까
	WeakPointer(TSharedPointer&& shared) = delete;


	~WeakPointer() {
		SubtractWeakCount(*this);
	}

	T& operator*() const {
		if (!Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return *m_Pointer;
	}

	T* operator->() const {
		if (!Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return m_Pointer;
	}


	T* Get() const {
		if (!Exist()) {
			return nullptr;
		}

		return m_Pointer;
	}

	Int32U RefCount() const {
		if (m_ControlBlock == nullptr) {
			return 0;
		}

		return m_ControlBlock->ReferenceCount;
	}

	Int32U WeakCount() const {
		if (m_ControlBlock == nullptr) {
			return 0;
		}

		return m_ControlBlock->WeakCount;
	}

	bool Exist() const {
		if (m_ControlBlock == nullptr) {
			return false;
		}

		return m_ControlBlock->ReferenceCount > 0;
	}

	TWeakPointer& operator=(std::nullptr_t ptr) {
		MakeEmpty();
		return *this;
	}

	TWeakPointer& operator=(TSharedPointer& other) {
		ChangeToShared(other);
		return *this;
	}

	TWeakPointer& operator=(TWeakPointer& other) {
		ChangeToWeak(other);
		return *this;
	}

	TWeakPointer& operator=(TWeakPointer&& other) {
		MoveToWeak(other);
		return *this;
	}

	TWeakPointer& operator=(TSharedPointer&& other) = delete;

	
private:
	void MakeEmpty() {
		SubtractWeakCount(*this);
		m_Pointer = nullptr;
		m_ControlBlock = nullptr;
	}

	void ChangeToWeak(TWeakPointer& weak) {
		SubtractWeakCount(*this);
		m_Pointer = weak.m_Pointer;
		m_ControlBlock = weak.m_ControlBlock;
		AddWeakCount(*this);
	}

	void MoveToWeak(TWeakPointer& weak) {
		SubtractWeakCount(*this);
		m_Pointer = weak.m_Pointer;
		m_ControlBlock = weak.m_ControlBlock;

		weak.m_Pointer = nullptr;
		weak.m_ControlBlock = nullptr;
	}

	void ChangeToShared(TSharedPointer& shared) {
		SubtractWeakCount(*this);
		m_Pointer = shared.m_Pointer;
		m_ControlBlock = shared.m_ControlBlock;
		AddWeakCount(*this);
	}

	static void AddWeakCount(TWeakPointer& shared) {
		if (shared.m_ControlBlock == nullptr) {
			return;
		}

		shared.m_ControlBlock->WeakCount++;
	}

	static void SubtractWeakCount(TWeakPointer& shared) {
		if (shared.m_ControlBlock == nullptr)
			return;

		if (shared.m_ControlBlock->WeakCount > 0)
			shared.m_ControlBlock->WeakCount--;

		if (shared.m_ControlBlock->WeakCount == 0) {
			DeleteSafe(shared.m_ControlBlock);
		}
	}
protected:
	T* m_Pointer = nullptr;
	ControlBlock* m_ControlBlock = nullptr;

	friend class TSharedPointer;
};


} // namespace JCore