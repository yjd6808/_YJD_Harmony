/*
	작성자 : 윤정도
	스마트 포인터 

	배열 타입까지 구현해버리는 바람에 다이나믹 캐스팅이
	실패하는 경우에 대한 처리를 하지 못했다.

	코딩을 신경써서 하는 수밖에..
	쓰레드 세이프하지 않으므로 세이프 버전을 만들든 해야할 듯
	<=============>
	
	UniqueObject<T>			단일 포인터 (동적할당 1회)
	UniqueObject<T[Size]>	컴파일타임에 크기가 결정되는 배열 포인터	(동적할당 1회)
	UniqueObject<T[]>		런타임에 크기가 결정되는 배열 포인터	(동적할당 2회 UniqueObject 생성할 때 1번, 내부에서 배열 동적할당 할 때 1번)
	
	SharedObject도 마찬가지로 방식으로 구현함
*/

#pragma once

#include <JCore/Declspec.h>
#include <JCore/Exception.h>
#include <JCore/Deletor.h>
#include <JCore/TypeCast.h>
#include <JCore/TypeTraits.h>
#include <JCore/Assert.h>

namespace JCore {
	namespace Detail {
		// 스마트포인터는 배열타입은 기본 타입으로 붕괴해서 체크하자.. ㅠ
		template <typename Lhs, typename Rhs>
		constexpr bool IsSmartPtrCastable_v = IsDynamicCastable_v<RemoveArray_t<Lhs>*, RemoveArray_t<Rhs>*>;

		template <typename Lhs, typename Rhs>
		void CheckDynamicCastable() {
			static_assert(IsSmartPtrCastable_v<Lhs, Rhs>, 
				"... cannot convert! Type T* and U* must be dynamic castable each other");
		}
	}



// 전방 선언
template <typename>	class SharedMaker;
template <typename>	class UniqueMaker;
template <typename>	class WeakPtr;
template <typename>	class SharedPtr;
template <typename>	class UniquePtr;

template <typename T, typename... Args>
constexpr decltype(auto) MakeShared(Args&&... args) {
	return SharedMaker<T>::Create(Forward<Args>(args)...);
}

template <typename T, typename... Args>
constexpr decltype(auto) MakeUnique(Args&&... args) {
	return UniqueMaker<T>::Create(Forward<Args>(args)...);
}


struct JCORE_NOVTABLE UniqueBase
{
	virtual ~UniqueBase() = default;
	virtual void DeleteSelf() = 0;
};

template <typename T>
struct UniqueObject : UniqueBase
{
	using TDeletor = PlacementDeletor<T, DeletorOption::OnlyDestoryObject>;

	template <typename... Args>
	explicit UniqueObject(Args&&... args) {
		::new (AddressOf(Object)) T(Forward<Args>(args)...);
	}

	~UniqueObject() override {}

	void DeleteSelf() override {
		TDeletor()(AddressOf(Object));
		delete this;
	}

	T* Address() const {
		return (T*)AddressOf(Object);
	}

	union { T Object; };
};

template <typename T, int Size>
struct UniqueObject<T[Size]> : UniqueBase
{
	using TDeletor = PlacementDeletor<T[Size], DeletorOption::OnlyDestoryObject>;

	template <typename... Args>
	explicit UniqueObject(Args&&... args) {
		for (int i = 0; i < Size; i++) {
			::new (AddressOf(Object[i])) T{ Forward<Args>(args)... };
		}
	}

	~UniqueObject() override {}

	void DeleteSelf() override {
		TDeletor()(Object);
		delete this;
	}

	T* Address() const {
		return (T*)Object;
	}

	union { T Object[Size]; };
};

template <typename T>
struct UniqueObject<T[]> : UniqueBase
{
	using TDeletor = PlacementDeletor<T[], DeletorOption::Both>;

	template <typename... Args>
	explicit UniqueObject(int Size, Args&&... args) {
		void* pRawMemory = operator new[](sizeof(T)* Size);
		Pointer = (T*)(pRawMemory);

		for (int i = 0; i < Size; i++) {
			::new (Pointer + i) T{ Forward<Args>(args)... };
		}

		m_Size = Size;
	}

	~UniqueObject() override {}

	void DeleteSelf() override {
		TDeletor()(Pointer, m_Size);
		delete this;
	}

	T* Address() const {
		return reinterpret_cast<T*>(Pointer);
	}

	union { T* Pointer; };
	int m_Size;
};

template <typename T>
class UniquePtr
{
	using TUniquePtr	= UniquePtr<T>;

	template <typename U>
	void SetUniquePtr(U* ptr, UniqueBase* base, int size) {
		m_Pointer = (T*)ptr;
		m_Base = base;
		m_Size = size;
	}
public:
	UniquePtr() : m_Pointer(nullptr) {}
	UniquePtr(std::nullptr_t nulptr) : m_Pointer(nullptr) {}

	template <typename U>
	UniquePtr(const UniquePtr<U>& other) = delete;

	template <typename U>
	UniquePtr(UniquePtr<U>&& other) {
		Detail::CheckDynamicCastable<U, T>();

		m_Pointer = (T*)other.m_Pointer;
		m_Base = other.m_Base;
		m_Size = other.m_Size;
		
		other.m_Pointer = nullptr;
		other.m_Base = nullptr;
		other.m_Size = 0;
		
	}

	virtual ~UniquePtr() { 
		if (m_Base != nullptr) {
			// 가상 클래스는 vfptr때문에 스칼라 타입은 4바이트만큼 뒤에 있어서 이렇게 캐스팅해주면 되던데
			// 배열로 만들면 달라지네;
			//UniqueBase* p = (UniqueBase*)((char*)m_Pointer - 4);

			m_Base->DeleteSelf();
		}
			
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


	T& operator[](const int idx) const {
		if (m_Pointer == nullptr) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		if (idx < 0 || idx >= m_Size) {
			throw OutOfRangeException("올바른 인덱스 값을 입력해주세요.");
		}

		return m_Pointer[idx];
	}

	int Length() const {
		return m_Size;
	}

	// Get<Model*> -> Model*로 반환
	// 모두 같은 반환을 수행한다.
	template <typename U = T*>
	U Get() const {
		if (m_Pointer == nullptr) {
			return nullptr;
		}

		
		static_assert(!IsReferenceType_v<U>, "... cannot cast to reference type");	// Get<int&>와 같은 캐스팅을 방지
		static_assert(IsPointerType_v<U>, "... only cast to pointer type.");		// Get<int>	와 같은 방식을 방지

		// U -> int* -> 포인터를 없애주고 비교해야함
		// T -> int
		Detail::CheckDynamicCastable<RemovePointer_t<U>, T>();
		return (U)m_Pointer;
	}

	template <typename AnyType>
	void operator=(AnyType&& other) = delete;
private:
	T* m_Pointer = nullptr;
	UniqueBase* m_Base = nullptr;
	int m_Size = 0;

	template <typename> friend class UniqueMaker;
	template <typename> friend class UniquePtr;
};






template <typename T>
class UniqueMaker
{
	static const int ms_uiArraySize = 1;		// 기본적으로 길이는 무조건 1

	using TUniquePtr = UniquePtr<T>;
	using TUniqueObject = UniqueObject<T>;
public:
	template <typename... Args>
	static constexpr TUniquePtr Create(Args&&... args) {
		TUniqueObject* obj = new TUniqueObject(Forward<Args>(args)...);
		TUniquePtr sp;
		sp.SetUniquePtr(obj->Address(), obj, ms_uiArraySize);
		return sp;
	}
};

template <typename T, int ArraySize>
class UniqueMaker<T[ArraySize]>
{
	using TUniquePtr = UniquePtr<T[ArraySize]>;
	using TUniqueObject = UniqueObject<T[ArraySize]>;
public:
	template <typename... Args>
	static constexpr TUniquePtr Create(Args&&... args) {
		TUniqueObject* obj = new TUniqueObject(Forward<Args>(args)...);
		TUniquePtr sp;
		sp.SetUniquePtr(obj->Address(), obj, ArraySize);
		return sp;
	}
};


template <typename T>
class UniqueMaker<T[]>
{
	using TUniquePtr = UniquePtr<T[]>;
	using TUniqueObject = UniqueObject<T[]>;
public:
	template <typename... Args>
	static constexpr TUniquePtr Create(int Size, Args&&... args) {
		TUniqueObject* obj = new TUniqueObject(Size, Forward<Args>(args)...);
		TUniquePtr sp;
		sp.SetUniquePtr(obj->Address(), obj, Size);
		return sp;
	}
};



struct JCORE_NOVTABLE ControlBlock
{	
	ControlBlock() = default;
	virtual ~ControlBlock() = default;

	virtual void DeleteSelf() = 0;
	virtual void DestroyObject() = 0;

	// 레퍼런스 카운트 올릴 때 위크 카운트도 같이 올려준다.
	void IncreaseRefCount() {
		ReferenceCount++;
		WeakCount++;
	}

	void IncreaseWeakCount() {
		WeakCount++;
	}

	void DecreaseRefCount() {
		ReferenceCount--;
		WeakCount--;				// 22-04-13 / WeakCount도 같이 빼줘야함

		if (ReferenceCount == 0)
			DestroyObject();
		if (WeakCount == 0)
			DeleteSelf();
	}

	void DecreaseWeakCount() {
		WeakCount--;
		if (WeakCount == 0) {
			DeleteSelf();
		}
	}


	int ReferenceCount = 1;
	int WeakCount = 1;
};


template <typename T>
struct SharedObject : ControlBlock
{
	using TDeletor = PlacementDeletor<T, DeletorOption::OnlyDestoryObject>;

	template <typename... Args>
	explicit SharedObject(Args&&... args) {
		::new (AddressOf(Object)) T(Forward<Args>(args)...);
	}

	~SharedObject() override {}

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


// 외부에서 직접 포인터를 넣어주는 경우때문에 추가해줌
template <typename T>
struct SharedObject<T*> : ControlBlock
{
	using TDeletor = PlacementDeletor<T, DeletorOption::OnlyDeletePointer>;

	explicit SharedObject(T* ptr) {
        Object = ptr;
	}

	~SharedObject() override {}

	void DestroyObject() override {
		TDeletor()(Object);
	}

	void DeleteSelf() override {
		delete this;
	}

	T* Address() const {
		return Object;
	}

    T* Object;
};

template <typename T, int Size>
struct SharedObject<T[Size]> : ControlBlock
{
	using TDeletor = PlacementDeletor<T[Size], DeletorOption::OnlyDestoryObject>;

	template <typename... Args>
	explicit SharedObject(Args&&... args) {
		for (int i = 0; i < Size; i++) {
			::new (AddressOf(Object[i])) T(Forward<Args>(args)...);
		}
	}

	~SharedObject() override {}

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
	using TDeletor = PlacementDeletor<T[], DeletorOption::Both>;

	template <typename... Args>
	explicit SharedObject(int Size, Args&&... args) {
		void* pRawMemory = operator new[](sizeof(T)* Size);
		Pointer = (T*)(pRawMemory);

		for (int i = 0; i < Size; i++) {
			::new (Pointer + i) T{ Forward<Args>(args)... };
		}

		m_Size = Size;
	}

	~SharedObject() override {}

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
	int m_Size;
};


template <typename T>
class BasePointer
{
	using TSharedPtr	= SharedPtr<T>;
	using TWeakPtr		= WeakPtr<T>;
public:
	int RefCount() const {
		if (m_pControlBlock == nullptr) {
			return 0;
		}

		return m_pControlBlock->ReferenceCount;
	}

	int WeakCount() const {
		if (m_pControlBlock == nullptr) {
			return 0;
		}

		return m_pControlBlock->WeakCount;
	}


	bool Exist() const {
		if (m_pControlBlock == nullptr)
			return false;

		return m_pControlBlock->ReferenceCount > 0;
	}

	T& GetObj() const {
		return *m_Pointer;
	}

	T* GetPtr() const {
		return m_Pointer;
	}

	int Length() const {
		return m_Size;
	}

	// Get<Model*> -> Model*로 반환
	// 모두 같은 반환을 수행한다.
	template <typename U = T*>
	U Get() const {
		if (!Exist()) {
			return nullptr;
		}

		static_assert(!IsReferenceType_v<U>, "... cannot cast to reference type");	// Get<int&>와 같은 캐스팅을 방지
		static_assert(IsPointerType_v<U>, "... only cast to pointer type.");		// Get<int>	와 같은 방식을 방지

		// U -> int* -> 포인터를 없애주고 비교해야함
		// T -> int
		Detail::CheckDynamicCastable<RemovePointer_t<U>, T>();
		return (U)m_Pointer;
	}

	T& operator*() const {
		if (!Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return GetObj();
	}

	T* operator->() const {
		if (!Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return GetPtr();
	}

	T& operator[](const int idx) const {
		if (!Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		if (idx < 0 || idx >= m_Size) {
			throw OutOfRangeException("올바른 인덱스 값을 입력해주세요.");
		}

		return m_Pointer[idx];
	}
protected:
	void MakeSharedEmpty() {
		m_pControlBlock->DecreaseRefCount();

		m_Pointer = nullptr;
		m_pControlBlock = nullptr;
		m_Size = 0;
	}

    void MakeShared(T* ptr) {
        // T*  => T*
	    m_pControlBlock = new SharedObject<T*>(ptr); 
        m_Pointer = ptr;
        m_Size = 1;
	}

	// Shared로 Shared 복사
	template <typename U>
	void SharedChangeToShared(SharedPtr<U>& shared) {
		SubtractReferenceCount();
		m_Pointer = (T*)(shared.m_Pointer);
		m_pControlBlock = shared.m_pControlBlock;
		m_Size = shared.m_Size;
		AddReferenceCount();
	}

	// Shared로 Weak 복사
	template <typename U>
	void SharedChangeToWeak(WeakPtr<U>& weak) {
		SubtractReferenceCount();
		m_Pointer = (T*)(weak.m_Pointer);
		m_pControlBlock = weak.m_pControlBlock;
		m_Size = weak.m_Size;
		AddReferenceCount();
	}

	// Weak로 Shared 복사
	template <typename U>
	void WeakChangeToShared(SharedPtr<U>& shared) {
		SubtractWeakCount();
		m_Pointer = (T*)(shared.m_Pointer);
		m_pControlBlock = shared.m_pControlBlock;
		m_Size = shared.m_Size;
		AddWeakCount();
	}

	// Weak로 Weak 복사
	template <typename U>
	void WeakChangeToWeak(WeakPtr<U>& weak) {
		SubtractWeakCount();
		m_Pointer = (T*)(weak.m_Pointer);
		m_pControlBlock = weak.m_pControlBlock;
		m_Size = weak.m_Size;
		AddWeakCount();
	}

	// Shared로 Shared 이동
	template <typename U>
	void SharedMoveToShared(SharedPtr<U>& shared) {
		SubtractReferenceCount();

		m_Pointer = (T*)(shared.m_Pointer);
		m_pControlBlock = shared.m_pControlBlock;
		m_Size = shared.m_Size;

		shared.m_Pointer = nullptr;
		shared.m_pControlBlock = nullptr;
		shared.m_Size = 0;
	}

	// Shared로 Weak 이동
	template <typename U>
	void SharedMoveToWeak(WeakPtr<U>& weak) {
		SubtractReferenceCount();
		m_Pointer = weak.m_Pointer;
		m_pControlBlock = weak.m_pControlBlock;

		weak.m_Pointer = nullptr;
		weak.m_pControlBlock = nullptr;
		weak.m_Size = 0;
	}

	template <typename U>
	static void WeakMoveToShared(SharedPtr<U>& shared) {
		// 필요 없음
		DebugAssertMessage(true, "멍미");
	}

	template <typename U>
	void WeakMoveToWeak(WeakPtr<U>& weak) {
		SubtractWeakCount();

		m_Pointer = weak.m_Pointer;
		m_pControlBlock = weak.m_pControlBlock;
		m_pControlBlock = weak.m_Size;

		weak.m_Pointer = nullptr;
		weak.m_pControlBlock = nullptr;
		weak.m_Size = 0;
	}


	void AddReferenceCount() const {
		if (m_pControlBlock == nullptr) {
			return;
		}

		m_pControlBlock->IncreaseRefCount();
	}

	void AddWeakCount() const {
		if (m_pControlBlock == nullptr) {
			return;
		}

		m_pControlBlock->IncreaseWeakCount();
	}

	void SubtractReferenceCount() const {
		if (m_pControlBlock == nullptr) {
			return;
		}

		// 레퍼런스 카운트 줄일때 위크 카운트도 같이 줄이도록 하므로 이 코드 없어야함 ㄷㄷ;
		// 이거땜에 계속 서버 팅겼나보다.. ㅠㅠ
		// 잘못된 메모리참조로 튕길때도 있고 안튕길때도 있어서 찾기가 넘 어려웠다. ㄷㄷ
		// m_ControlBlock->DecreaseWeakCount();
		m_pControlBlock->DecreaseRefCount();
		
	}

	void SubtractWeakCount() const {
		if (m_pControlBlock == nullptr) {
			return;
		}

		m_pControlBlock->DecreaseWeakCount();
	}

	template <typename U>
	void SetSharedPtr(U* ptr, ControlBlock* controlBlock, int size) {

//		SharedPtr<Model[]> p3 = MakeShared<Model2[]>(20); // 모델 객체 20의 배열 생성
// 		배열타입들땜에 강제 형변환 해줘야한다. ㄷㄷ;
// 
//		m_Pointer = static_cast<T*>(ptr);
//		std::cout << Type_v<T> << "\n";
//		std::cout << Type_v<U> << "\n";

		m_Pointer = (T*)(ptr);
		m_pControlBlock = controlBlock;
		m_Size = size;
	}

protected:
	T* m_Pointer = nullptr;
	ControlBlock* m_pControlBlock = nullptr;
	int m_Size = 0;

	template <typename> friend class SharedMaker;
	template <typename> friend class BasePointer;
};


template <typename T>
class SharedPtr : public BasePointer<T>
{
	using TSharedPtr = SharedPtr<T>;
public:
	SharedPtr() {}
	SharedPtr(std::nullptr_t nulptr) {}

    // 외부에서 포인터를 직접 넣어주는 경우
    // 밖에서 포인터 생성하고 안에서 또 제어블록을 생성해서 비효율적이지만
    // 생성자가 private으로 선언된 경우 이 방식으로 초기화해야함
    // 
    // ┌ SharedPtr<int[20]>(new int);  ==> 지원안함
    // │ SharedPtr<int[]>(new int[20]);==> 지원안함
    // └ SharedPtr<int>(new int);      ==> 이거만 가능
    //
    // 진짜 라이브러리 만드는게 쉬운게 아니구나;; 혼자서는 이런거 하나하나 다 캐치해낼 수가 없다.

    SharedPtr(T* ptr) {
        this->MakeShared(ptr);
	}

	template <typename U>
	SharedPtr(WeakPtr<U>& weak) {
		Detail::CheckDynamicCastable<U, T>();
		this->SharedChangeToWeak(weak);
	}
	
	template <typename U>
	SharedPtr(WeakPtr<U>&& weak) {
		Detail::CheckDynamicCastable<U, T>();
		this->SharedMoveToWeak(weak);
	}

	template <typename U>
	SharedPtr(SharedPtr<U>& shared) {
		Detail::CheckDynamicCastable<U, T>();
		this->SharedChangeToShared(shared);
	}

	template <typename U>
	SharedPtr(SharedPtr<U>&& shared) {
		Detail::CheckDynamicCastable<U, T>();
		this->SharedMoveToShared(shared);
	}

	~SharedPtr() {
		this->SubtractReferenceCount();
	}
	
	TSharedPtr& operator=(std::nullptr_t ptr) {
		this->MakeSharedEmpty();
		return *this;
	}

	template <typename U>
	TSharedPtr& operator=(SharedPtr<U>& other) {
		Detail::CheckDynamicCastable<U, T>();
		this->SharedChangeToShared(other);
		return *this;
	}

	template <typename U>
	TSharedPtr& operator=(SharedPtr<U>&& other) {
		Detail::CheckDynamicCastable<U, T>();
		this->SharedMoveToShared(other);
		return *this;
	}

	template <typename U>
	TSharedPtr& operator=(WeakPtr<U>& other) {
		Detail::CheckDynamicCastable<U, T>();
		this->SharedChangeToWeak(other);
		return *this;
	}

	template <typename U>
	TSharedPtr& operator=(WeakPtr<U>&& other) {
		Detail::CheckDynamicCastable<U, T>();
		this->SharedMoveToWeak(other);
		return *this;
	}
};


template <typename T>
class WeakPtr : public BasePointer<T>
{
	using TWeakPtr = WeakPtr<T>;
public:
	WeakPtr() {}
	WeakPtr(std::nullptr_t nulptr) {}

	template <typename U>
	WeakPtr(WeakPtr<U>& weak) {
		Detail::CheckDynamicCastable<U, T>();
		this->WeakChangeToWeak(weak);
	}

	template <typename U>
	WeakPtr(WeakPtr<U>&& weak) {
		Detail::CheckDynamicCastable<U, T>();
		this->WeakMoveToWeak(weak);
	}

	template <typename U>
	WeakPtr(SharedPtr<U>& shared) {
		Detail::CheckDynamicCastable<U, T>();
		this->WeakChangeToShared(shared);
	}

	// 쉐어드 포인터의 이동은 막도록 하자.
	// 본체가 사라진 녀석을 참조해버릴 수 있으니까
	template <typename U>
	WeakPtr(SharedPtr<U>&& shared) = delete;


	~WeakPtr() {
		this->SubtractWeakCount();
	}

	TWeakPtr& operator=(std::nullptr_t ptr) {
		this->MakeWeakEmpty();
		return *this;
	}


	template <typename U>
	TWeakPtr& operator=(SharedPtr<U>& other) {
		Detail::CheckDynamicCastable<U, T>();
		this->WeakChangeToShared(other);
		return *this;
	}

	template <typename U>
	TWeakPtr& operator=(WeakPtr<U>& other) {
		Detail::CheckDynamicCastable<U, T>();
		this->WeakChangeToWeak(other);
		return *this;
	}

	template <typename U>
	TWeakPtr& operator=(WeakPtr<U>&& other) {
		Detail::CheckDynamicCastable<U, T>();
		this->WeakMoveToWeak(other);
		return *this;
	}

	template <typename U>
	TWeakPtr& operator=(SharedPtr<U>&& other) = delete;
};


template <typename T>
class SharedMaker
{
	static const int ms_uiArraySize = 1;		// 기본적으로 길이는 무조건 1

	using TSharedPtr = SharedPtr<T>;
	using TSharedObject = SharedObject<T>;
public:
	template <typename... Args>
	static constexpr TSharedPtr Create(Args&&... args) {
		TSharedObject* obj = new TSharedObject(Forward<Args>(args)...);
		TSharedPtr sp;
		sp.SetSharedPtr(obj->Address(), obj, ms_uiArraySize);
		return sp;
	}
};

template <typename T, int ArraySize>
class SharedMaker<T[ArraySize]>
{
	using TSharedPtr = SharedPtr<T[ArraySize]>;
	using TSharedObject = SharedObject<T[ArraySize]>;
public:
	template <typename... Args>
	static constexpr TSharedPtr Create(Args&&... args) {
		TSharedObject* obj = new TSharedObject(Forward<Args>(args)...);
		TSharedPtr sp;
		sp.SetSharedPtr(obj->Address(), obj, ArraySize);
		return sp;
	}
};


template <typename T>
class SharedMaker<T[]>
{
	using TSharedPtr = SharedPtr<T[]>;
	using TSharedObject = SharedObject<T[]>;
public:
	template <typename... Args>
	static constexpr TSharedPtr Create(int size, Args&&... args) {
		TSharedObject* obj = new TSharedObject(size, Forward<Args>(args)...);
		TSharedPtr sp;
		sp.SetSharedPtr(obj->Address(), obj, size);
		return sp;
	}
};











// 글로벌 비교 오퍼레이터

// U == U
// U == S
// S == U
// U == W
// W == U
// S == S
// S == W
// W == S
// W == W

template <typename T, typename U>
bool operator==(const UniquePtr<T>& lhs, const UniquePtr<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const UniquePtr<T>& lhs, const SharedPtr<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const SharedPtr<T>& lhs, const UniquePtr<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const UniquePtr<T>& lhs, const WeakPtr<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const WeakPtr<T>& lhs, const UniquePtr<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const SharedPtr<T>& lhs, const SharedPtr<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const SharedPtr<T>& lhs, const WeakPtr<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const WeakPtr<T>& lhs, const SharedPtr<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const WeakPtr<T>& lhs, const WeakPtr<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const UniquePtr<T>& lhs, std::nullptr_t) {
	return lhs.Get() == nullptr;
}

template <typename T, typename U>
bool operator==(std::nullptr_t, const UniquePtr<T>& rhs) {
	return nullptr == rhs.Get();
}

template <typename T, typename U>
bool operator==(const SharedPtr<T>& lhs, std::nullptr_t) {
	return lhs.Get() == nullptr;
}

template <typename T, typename U>
bool operator==(std::nullptr_t, const SharedPtr<T>& rhs) {
	return nullptr == rhs.Get();
}

template <typename T, typename U>
bool operator==(const WeakPtr<T>& lhs, std::nullptr_t) {
	return lhs.Get() == nullptr;
}

template <typename T, typename U>
bool operator==(std::nullptr_t, const WeakPtr<T>& rhs) {
	return nullptr == rhs.Get();
}












} // namespace JCore