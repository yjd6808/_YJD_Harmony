/*
	작성자 : 윤정도
	스마트 포인터 만들어보기
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Exception.h>
#include <JCore/Deletor.h>
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

#ifndef DeleteArraySafe
#define DeleteArraySafe(x)		\
do {							\
	if (x) {					\
		delete[] x;				\
	}							\
	x = nullptr;				\
} while (0);		
#endif

namespace JCore {
	// 스마트포인터는 배열타입은 기본 타입으로 붕괴해서 체크하자.. ㅠ
	// 원시 타입인 경우는 같은 타입끼리만 가능하도록 한다.
	template <typename Lhs, typename Rhs>
	constexpr bool SmartPointer_Castable_v 
			= (IsPrimitiveType_v<RemoveArray_t<Lhs>> && IsPrimitiveType_v<RemoveArray_t<Rhs>>) ? 
			  IsSameType_v<RemoveArray_t<Lhs>, RemoveArray_t<Rhs>> : 
			  DynamicCastable_v<RemoveArray_t<Lhs>*, RemoveArray_t<Rhs>*>;

// 전방 선언
template <typename>	class SharedMaker;
template <typename>	class WeakPointer;
template <typename>	class SharedPointer;
template <typename>	class UniquePointer;

template <typename T, typename... Args>
constexpr decltype(auto) MakeShared(Args&&... args) {
	return SharedMaker<T>::Create(Forward<Args>(args)...);
}

template <typename T, typename... Args>
constexpr decltype(auto) MakeUnique(Args&&... args) {
	return UniquePointer<T>::Create(Forward<Args>(args)...);
}

template <typename T>
class UniquePointer
{
	using TDeletor			= typename Deletor<T>;
	using TUniquePointer	= typename UniquePointer<T>;

	UniquePointer(T* ptr) : m_Pointer(ptr) {}
public:
	UniquePointer() : m_Pointer(nullptr) {}
	UniquePointer(std::nullptr_t nulptr) : m_Pointer(nullptr) {}
	UniquePointer(const TUniquePointer& other) = delete;
	UniquePointer(TUniquePointer&& other) {
		m_Pointer = other.m_Pointer;
		other.m_Pointer = nullptr;
	}

	virtual ~UniquePointer() { TDeletor()(m_Pointer); }

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

	template <typename... Args>
	constexpr static TUniquePointer Create(Args&&... args) {
		return TUniquePointer{ new T{ Forward<Args>(args)... } };
	}
private:
	T* m_Pointer;
};


template <typename T>
class UniquePointer<T[]>
{
	using TDeleteor			= typename PlacementDeletor<T[]>;
	using TUniquePointer	= typename UniquePointer<T[]>;

	UniquePointer(T* ptr, Int32U arraySize) : m_Pointer(ptr), m_Size(arraySize) {}
public:
	UniquePointer() : m_Pointer(nullptr) {}
	UniquePointer(std::nullptr_t nulptr) : m_Pointer(nullptr) {}
	UniquePointer(const TUniquePointer& other) = delete;
	UniquePointer(TUniquePointer&& other) {
		m_Pointer = other.m_Pointer;
		other.m_Pointer = nullptr;
	}

	virtual ~UniquePointer() { 
		TDeleteor()(m_Pointer, m_Size);
	}

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

	T& operator[](const int idx) {
		if (idx < 0 || idx >= m_Size) {
			throw RuntimeException("인덱스 범위를 벗어났습니다.");
		}
		return m_Pointer[idx];
	}

	template <typename AnyType>
	void operator=(AnyType&& other) = delete;

	template <typename... Args>
	constexpr static TUniquePointer Create(Int32U Size, Args&&... args) {
		void* pRawMemory = operator new[](sizeof(T) * Size);
		T* pArray = (T*)(pRawMemory);

		for (int i = 0; i < Size; i++) {
			::new (pArray + i) T{ Forward<Args>(args)...};
		}

		return TUniquePointer(pArray, Size);
	}
private:
	T* m_Pointer;
	Int32U m_Size;
};

template <typename T, Int32U ArraySize>
class UniquePointer<T[ArraySize]>
{
	using TDeleteor		 = typename PlacementDeletor<T[ArraySize]>;
	using TUniquePointer = typename UniquePointer<T[ArraySize]>;

	UniquePointer(T* ptr) : m_Pointer(ptr) {}
public:
	UniquePointer() : m_Pointer(nullptr) {}
	UniquePointer(std::nullptr_t nulptr) : m_Pointer(nullptr) {}
	UniquePointer(const TUniquePointer& other) = delete;
	UniquePointer(TUniquePointer&& other) {
		m_Pointer = other.m_Pointer;
		other.m_Pointer = nullptr;
	}

	virtual ~UniquePointer() {
		TDeleteor()(m_Pointer);
	}

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

	T& operator[](const int idx) {
		if (idx < 0 || idx >= ArraySize) {
			throw RuntimeException("인덱스 범위를 벗어났습니다.");
		}
		return m_Pointer[idx];
	}

	template <typename AnyType>
	void operator=(AnyType&& other) = delete;

	template <typename... Args>
	constexpr static TUniquePointer Create(Args&&... args) {
		void* pRawMemory = operator new[](sizeof(T) * ArraySize);
		T* pArray = (T*)(pRawMemory);

		for (int i = 0; i < ArraySize; i++) {
			::new (pArray + i) T{ Forward<Args>(args)... };
		}

		return TUniquePointer(pArray);
	}
private:
	T* m_Pointer;
};




struct __declspec(novtable) ControlBlock
{	
	ControlBlock() {}
	virtual ~ControlBlock() {}

	virtual void DeleteSelf() = 0;
	virtual void DestroyObject() = 0;

	void IncreaseRefCount() {
		ReferenceCount.fetch_add(1);
	}

	void IncreaseWeakCount() {
		WeakCount.fetch_add(1);
	}

	void DecreaseRefCount() {
		ReferenceCount--;
		if (ReferenceCount == 0) {
			DestroyObject();
			DecreaseWeakCount();
		}
	}

	void DecreaseWeakCount() {
		WeakCount--;
		if (WeakCount == 0) {
			DeleteSelf();
		}
	}


	std::atomic<Int32U> ReferenceCount = 1;
	std::atomic<Int32U> WeakCount = 1;
};


template <typename T>
struct SharedObject : ControlBlock
{
	using TDeletor = typename PlacementDeletor<T, DeletorOption::OnlyDestoryObject>;

	template <typename... Args>
	explicit SharedObject(Args&&... args) {
		::new (reinterpret_cast<void*>(AddressOf(Object))) T(Forward<Args>(args)...);
	}
	virtual ~SharedObject() {}

	void DestroyObject() override {
		TDeletor()(AddressOf(Object));
	}

	void DeleteSelf() override {
		delete this;
	}

	T* Address() const {
		return (T*)AddressOf(Object);
	}

	union { T Object; };
};

template <typename T, Int32U Size>
struct SharedObject<T[Size]> : ControlBlock
{
	using TDeletor = typename PlacementDeletor<T[Size], DeletorOption::OnlyDestoryObject>;

	template <typename... Args>
	explicit SharedObject(Args&&... args) {
		for (int i = 0; i < Size; i++) {
			::new (AddressOf(Object[i])) T(Forward<Args>(args)...);
		}
	}

	virtual ~SharedObject() {}

	void DestroyObject() override {
		TDeletor()(Object);
	}

	void DeleteSelf() override {
		delete this;
	}

	T* Address() const {
		return (T*)Object;
	}

	union { T Object[Size]; };
};

template <typename T>
struct SharedObject<T[]> : ControlBlock
{
	using TDeletor = typename PlacementDeletor<T[], DeletorOption::Both>;

	template <typename... Args>
	explicit SharedObject(int Size, Args&&... args) {
		void* pRawMemory = ::operator new[](sizeof(T)* Size);
		Pointer = (T*)(pRawMemory);

		for (int i = 0; i < Size; i++) {
			::new (Pointer + i) T{ Forward<Args>(args)... };
		}

		m_Size = Size;
	}
	virtual ~SharedObject() {}

	void DestroyObject() override {
		TDeletor()(Pointer, m_Size);
	}

	void DeleteSelf() override {
		delete this;
	}

	T* Address() const {
		return reinterpret_cast<T*>(Pointer);
	}

	union { T* Pointer; };
	Int32U m_Size;
};


template <typename T>
class BasePointer
{
	using TSharedPointer	= typename SharedPointer<T>;
	using TWeakPointer		= typename WeakPointer<T>;
public:
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
		if (m_ControlBlock == nullptr)
			return false;

		return m_ControlBlock->ReferenceCount > 0;
	}

	T& GetObj() const {
		return *m_Pointer;
	}

	T* GetPointer() const {
		return m_Pointer;
	}
protected:
	void MakeSharedEmpty() {
		m_ControlBlock->DecreaseRefCount();

		m_Pointer = nullptr;
		m_ControlBlock = nullptr;
	}

	// Shared로 Shared 복사
	template <typename U>
	void SharedChangeToShared(SharedPointer<U>& shared) {
		SubtractReferenceCount();
		m_Pointer = (T*)(shared.m_Pointer);
		m_ControlBlock = shared.m_ControlBlock;
		AddReferenceCount();
	}

	// Shared로 Weak 복사
	template <typename U>
	void SharedChangeToWeak(WeakPointer<U>& weak) {
		SubtractReferenceCount();
		m_Pointer = (T*)(weak.m_Pointer);
		m_ControlBlock = weak.m_ControlBlock;
		AddReferenceCount();
	}

	// Weak로 Shared 복사
	template <typename U>
	void WeakChangeToShared(SharedPointer<U>& shared) {
		SubtractWeakCount();
		m_Pointer = (T*)(shared.m_Pointer);
		m_ControlBlock = shared.m_ControlBlock;
		AddWeakCount();
	}

	// Weak로 Weak 복사
	template <typename U>
	void WeakChangeToWeak(WeakPointer<U>& weak) {
		SubtractWeakCount();
		m_Pointer = (T*)(weak.m_Pointer);
		m_ControlBlock = weak.m_ControlBlock;
		AddWeakCount();
	}

	// Shared로 Shared 이동
	template <typename U>
	void SharedMoveToShared(SharedPointer<U>& shared) {
		SubtractReferenceCount();

		m_Pointer = (T*)(shared.m_Pointer);
		m_ControlBlock = shared.m_ControlBlock;

		shared.m_Pointer = nullptr;
		shared.m_ControlBlock = nullptr;
	}

	// Shared로 Weak 이동
	template <typename U>
	void SharedMoveToWeak(WeakPointer<U>& weak) {
		SubtractReferenceCount();
		m_Pointer = weak.m_Pointer;
		m_ControlBlock = weak.m_ControlBlock;

		weak.m_Pointer = nullptr;
		weak.m_ControlBlock = nullptr;
	}

	template <typename U>
	void WeakMoveToShared(SharedPointer<U>& shared) {
		// 필요 없음
		DebugAssert(true, "멍미");
	}

	template <typename U>
	void WeakMoveToWeak(WeakPointer<U>& weak) {
		SubtractWeakCount();

		m_Pointer = weak.m_Pointer;
		m_ControlBlock = weak.m_ControlBlock;

		weak.m_Pointer = nullptr;
		weak.m_ControlBlock = nullptr;
	}


	void AddReferenceCount() {
		if (m_ControlBlock == nullptr) {
			return;
		}

		m_ControlBlock->IncreaseRefCount();
		m_ControlBlock->IncreaseWeakCount();
	}

	void AddWeakCount() {
		if (m_ControlBlock == nullptr) {
			return;
		}

		m_ControlBlock->IncreaseWeakCount();
	}

	void SubtractReferenceCount() {
		if (m_ControlBlock == nullptr) {
			return;
		}

		m_ControlBlock->DecreaseRefCount();
		m_ControlBlock->DecreaseWeakCount();
	}

	void SubtractWeakCount() {
		if (m_ControlBlock == nullptr) {
			return;
		}

		m_ControlBlock->DecreaseWeakCount();
	}

	template <typename U>
	void SetSharedPointer(U* ptr, ControlBlock* controlBlock) {

//		SharedPointer<Model[]> p3 = MakeShared<Model2[]>(20); // 모델 객체 20의 배열 생성
// 		배열타입들땜에 강제 형변환 해줘야한다. ㄷㄷ;
// 
//		m_Pointer = static_cast<T*>(ptr);
//		std::cout << Type_v<T> << "\n";
//		std::cout << Type_v<U> << "\n";

		m_Pointer = (T*)(ptr);
		m_ControlBlock = controlBlock;
	}

protected:
	T* m_Pointer = nullptr;
	ControlBlock* m_ControlBlock = nullptr;

	friend class SharedMaker<T>;

	template <typename U>
	friend class BasePointer;
};


template <typename T>
class SharedPointer : public BasePointer<T>
{
	using TSharedPointer = typename SharedPointer<T>;
public:
	SharedPointer() {}
	SharedPointer(std::nullptr_t nulptr) {}

	template <typename U>
	SharedPointer(WeakPointer<U>& weak) {
		static_assert(SmartPointer_Castable_v<U, T>, "... cannot convert");
		this->SharedChangeToWeak(weak);
	}
	
	template <typename U>
	SharedPointer(WeakPointer<U>&& weak) {
		static_assert(SmartPointer_Castable_v<U, T>, "... cannot convert");
		this->SharedMoveToWeak(weak);
	}

	template <typename U>
	SharedPointer(SharedPointer<U>& shared) {
		static_assert(SmartPointer_Castable_v<U, T>, "... cannot convert");
		this->SharedChangeToShared(shared);
	}

	template <typename U>
	SharedPointer(SharedPointer<U>&& shared) {
		static_assert(SmartPointer_Castable_v<U, T>, "... cannot convert");
		this->SharedMoveToShared(shared);
	}

	~SharedPointer() {
		this->SubtractReferenceCount();
	}

	T& operator*() const {
		if (!this->Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return this->GetObj();
	}

	T* operator->() const {
		if (!this->Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return this->GetPointer();
	}

	T* Get() const {
		if (!this->Exist()) {
			return nullptr;
		}

		return this->GetPointer();
	}


	TSharedPointer& operator=(std::nullptr_t ptr) {
		this->MakeSharedEmpty();
		return *this;
	}

	template <typename U>
	TSharedPointer& operator=(SharedPointer<U>& other) {
		static_assert(SmartPointer_Castable_v<U, T>, "... cannot convert");
		this->SharedChangeToShared(other);
		return *this;
	}

	template <typename U>
	TSharedPointer& operator=(SharedPointer<U>&& other) {
		static_assert(SmartPointer_Castable_v<U, T>, "... cannot convert");
		this->SharedMoveToShared(other);
		return *this;
	}

	template <typename U>
	TSharedPointer& operator=(WeakPointer<U>& other) {
		static_assert(SmartPointer_Castable_v<U, T>, "... cannot convert");
		this->SharedChangeToWeak(other);
		return *this;
	}

	template <typename U>
	TSharedPointer& operator=(WeakPointer<U>&& other) {
		static_assert(SmartPointer_Castable_v<U, T>, "... cannot convert");
		this->SharedMoveToWeak(other);
		return *this;
	}
};

template <typename T>
class SharedMaker
{
	using TSharedPointer	= typename SharedPointer<T>;
	using TSharedObject		= typename SharedObject<T>;
public:
	template <typename... Args>
	static constexpr TSharedPointer Create(Args&&... args) {
		TSharedObject* obj = new TSharedObject(Forward<Args>(args)...);
		TSharedPointer sp;
		sp.SetSharedPointer(obj->Address(), obj);
		return sp;
	}
};

template <typename T, Int32U ArraySize>
class SharedMaker<T[ArraySize]>
{
	using TSharedPointer = typename SharedPointer<T[ArraySize]>;
	using TSharedObject = typename SharedObject<T[ArraySize]>;
public:
	template <typename... Args>
	static constexpr TSharedPointer Create(Args&&... args) {
		TSharedObject* obj = new TSharedObject(Forward<Args>(args)...);
		TSharedPointer sp;
		sp.SetSharedPointer(obj->Address(), obj);
		return sp;
	}
};


template <typename T>
class SharedMaker<T[]>
{
	using TSharedPointer = typename SharedPointer<T[]>;
	using TSharedObject = typename SharedObject<T[]>;
public:
	template <typename... Args>
	static constexpr TSharedPointer Create(Int32U Size, Args&&... args) {
		TSharedObject* obj = new TSharedObject(Size, Forward<Args>(args)...);
		TSharedPointer sp;
		sp.SetSharedPointer(obj->Address(), obj);
		return sp;
	}
};


template <typename T>
class WeakPointer : public BasePointer<T>
{
	using TWeakPointer = typename WeakPointer<T>;
public:
	WeakPointer() {}
	WeakPointer(std::nullptr_t nulptr) {}

	template <typename U>
	WeakPointer(WeakPointer<U>& weak) {
		static_assert(SmartPointer_Castable_v<U, T>, "... cannot convert");
		this->WeakChangeToWeak(weak);
	}

	template <typename U>
	WeakPointer(WeakPointer<U>&& weak) {
		static_assert(SmartPointer_Castable_v<U, T>, "... cannot convert");
		this->WeakMoveToWeak(weak);
	}

	template <typename U>
	WeakPointer(SharedPointer<U>& shared) {
		static_assert(SmartPointer_Castable_v<U, T>, "... cannot convert");
		this->WeakChangeToShared(shared);
	}

	// 쉐어드 포인터의 이동은 막도록 하자.
	// 본체가 사라진 녀석을 참조해버릴 수 있으니까
	template <typename U>
	WeakPointer(SharedPointer<U>&& shared) = delete;


	~WeakPointer() {
		this->SubtractWeakCount();
	}

	T& operator*() const {
		if (!this->Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return this->GetObj();
	}

	T* operator->() const {
		if (!this->Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return this->GetPointer();
	}

	T* Get() const {
		if (!this->Exist()) {
			return nullptr;
		}

		return this->GetPointer();
	}


	TWeakPointer& operator=(std::nullptr_t ptr) {
		this->MakeWeakEmpty();
		return *this;
	}


	template <typename U>
	TWeakPointer& operator=(SharedPointer<U>& other) {
		static_assert(SmartPointer_Castable_v<U, T>, "... cannot convert");
		this->WeakChangeToShared(other);
		return *this;
	}

	template <typename U>
	TWeakPointer& operator=(WeakPointer<U>& other) {
		static_assert(SmartPointer_Castable_v<U, T>, "... cannot convert");
		this->WeakChangeToWeak(other);
		return *this;
	}

	template <typename U>
	TWeakPointer& operator=(WeakPointer<U>&& other) {
		static_assert(SmartPointer_Castable_v<U, T>, "... cannot convert");
		this->WeakMoveToWeak(other);
		return *this;
	}

	template <typename U>
	TWeakPointer& operator=(SharedPointer<U>&& other) = delete;
};


} // namespace JCore