/*
	작성자 : 윤정도
	스마트 포인터 만들어보기

	배열 타입까지 구현해버리는 바람에 다이나믹 캐스팅이
	실패하는 경우에 대한 처리를 하지 못했다.

	코딩을 신경써서 하는 수밖에..
	쓰레드 세이프하지 않으므로 나중에 뮤텍스 버전을 만들든 해야할 듯

	<=============>
	
	면접시 설명을 다 할 수 있어야하기 땜에 작성한 코드 다시 한번씩 보는 중;
	배열 스마트 포인터 추가로 코드가 바로바로 머리속으로 안들어옴 그래서 정리를 좀 한다.
	22/05/14
	
	UniqueObject<T>			단일 포인터 (동적할당 1회)
	UniqueObject<T[Size]>	컴파일타임에 크기가 결정되는 배열 포인터	(동적할당 1회)
	UniqueObject<T[]>		런타임에 크기가 결정되는 배열 포인터	(동적할당 2회 UniqueObject 생성할 때 1번, 내부에서 배열 동적할당 할 때 1번)
	
	SharedObject도 마찬가지로 방식으로 구현함
*/

#pragma once

#include <JCore/Exception.h>
#include <JCore/Deletor.h>
#include <JCore/TypeTraits.h>

#include <cassert>

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


#ifndef DebugAssert
	#define DebugAssert(exp, msg)		assert((exp) && msg)
#endif

namespace JCore {
	namespace StaticAssert {
		// 스마트포인터는 배열타입은 기본 타입으로 붕괴해서 체크하자.. ㅠ
		template <typename Lhs, typename Rhs>
		constexpr bool SmartPointer_Castable_v = DynamicCastable_v<RemoveArray_t<Lhs>*, RemoveArray_t<Rhs>*>;

		// 안씀
		template <typename From, typename To>
		constexpr bool SmartPointer_BaseOf_v = IsRPBasedOf_v<RemoveArray_t<From>*, RemoveArray_t<To>*>;

		template <typename Lhs, typename Rhs>
		void CheckDynamicCastable() {
			static_assert(SmartPointer_Castable_v<Lhs, Rhs>, 
				"... cannot convert! Type T* and U* must be dynamic castable each other");
		}
	}



// 전방 선언
template <typename>	class SharedMaker;
template <typename>	class UniqueMaker;
template <typename>	class WeakPointer;
template <typename>	class SharedPointer;
template <typename>	class UniquePointer;

template <typename T, typename... Args>
constexpr decltype(auto) MakeShared(Args&&... args) {
	return SharedMaker<T>::Create(Forward<Args>(args)...);
}

template <typename T, typename... Args>
constexpr decltype(auto) MakeUnique(Args&&... args) {
	return UniqueMaker<T>::Create(Forward<Args>(args)...);
}


struct __declspec(novtable) UniqueBase
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
		void* pRawMemory = ::operator new[](sizeof(T)* Size);
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
class UniquePointer
{
	using TUniquePointer	= UniquePointer<T>;

	template <typename U>
	void SetUniquePointer(U* ptr, UniqueBase* base, int size) {
		m_Pointer = (T*)ptr;
		m_Base = base;
		m_Size = size;
	}
public:
	UniquePointer() : m_Pointer(nullptr) {}
	UniquePointer(std::nullptr_t nulptr) : m_Pointer(nullptr) {}

	template <typename U>
	UniquePointer(const UniquePointer<U>& other) = delete;

	template <typename U>
	UniquePointer(UniquePointer<U>&& other) {
		StaticAssert::CheckDynamicCastable<U, T>();

		m_Pointer = (T*)other.m_Pointer;
		m_Base = other.m_Base;
		m_Size = other.m_Size;
		
		other.m_Pointer = nullptr;
		other.m_Base = nullptr;
		other.m_Size = 0;
		
	}

	virtual ~UniquePointer() { 
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
		StaticAssert::CheckDynamicCastable<RemovePointer_t<U>, T>();
		return (U)m_Pointer;
	}

	template <typename AnyType>
	void operator=(AnyType&& other) = delete;
private:
	T* m_Pointer = nullptr;
	UniqueBase* m_Base = nullptr;
	int m_Size = 0;

	template <typename> friend class UniqueMaker;
	template <typename> friend class UniquePointer;
};






template <typename T>
class UniqueMaker
{
	static const int ms_uiArraySize = 1;		// 기본적으로 길이는 무조건 1

	using TUniquePointer = UniquePointer<T>;
	using TUniqueObject = UniqueObject<T>;
public:
	template <typename... Args>
	static constexpr TUniquePointer Create(Args&&... args) {
		TUniqueObject* obj = new TUniqueObject(Forward<Args>(args)...);
		TUniquePointer sp;
		sp.SetUniquePointer(obj->Address(), obj, ms_uiArraySize);
		return sp;
	}
};

template <typename T, int ArraySize>
class UniqueMaker<T[ArraySize]>
{
	using TUniquePointer = UniquePointer<T[ArraySize]>;
	using TUniqueObject = UniqueObject<T[ArraySize]>;
public:
	template <typename... Args>
	static constexpr TUniquePointer Create(Args&&... args) {
		TUniqueObject* obj = new TUniqueObject(Forward<Args>(args)...);
		TUniquePointer sp;
		sp.SetUniquePointer(obj->Address(), obj, ArraySize);
		return sp;
	}
};


template <typename T>
class UniqueMaker<T[]>
{
	using TUniquePointer = UniquePointer<T[]>;
	using TUniqueObject = UniqueObject<T[]>;
public:
	template <typename... Args>
	static constexpr TUniquePointer Create(int Size, Args&&... args) {
		TUniqueObject* obj = new TUniqueObject(Size, Forward<Args>(args)...);
		TUniquePointer sp;
		sp.SetUniquePointer(obj->Address(), obj, Size);
		return sp;
	}
};



struct __declspec(novtable)  ControlBlock
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
		void* pRawMemory = ::operator new[](sizeof(T)* Size);
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
	using TSharedPointer	= SharedPointer<T>;
	using TWeakPointer		= WeakPointer<T>;
public:
	int RefCount() const {
		if (m_ControlBlock == nullptr) {
			return 0;
		}

		return m_ControlBlock->ReferenceCount;
	}

	int WeakCount() const {
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
		StaticAssert::CheckDynamicCastable<RemovePointer_t<U>, T>();
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

		return GetPointer();
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
		m_ControlBlock->DecreaseRefCount();

		m_Pointer = nullptr;
		m_ControlBlock = nullptr;
		m_Size = 0;
	}

	// Shared로 Shared 복사
	template <typename U>
	void SharedChangeToShared(SharedPointer<U>& shared) {
		SubtractReferenceCount();
		m_Pointer = (T*)(shared.m_Pointer);
		m_ControlBlock = shared.m_ControlBlock;
		m_Size = shared.m_Size;
		AddReferenceCount();
	}

	// Shared로 Weak 복사
	template <typename U>
	void SharedChangeToWeak(WeakPointer<U>& weak) {
		SubtractReferenceCount();
		m_Pointer = (T*)(weak.m_Pointer);
		m_ControlBlock = weak.m_ControlBlock;
		m_Size = weak.m_Size;
		AddReferenceCount();
	}

	// Weak로 Shared 복사
	template <typename U>
	void WeakChangeToShared(SharedPointer<U>& shared) {
		SubtractWeakCount();
		m_Pointer = (T*)(shared.m_Pointer);
		m_ControlBlock = shared.m_ControlBlock;
		m_Size = shared.m_Size;
		AddWeakCount();
	}

	// Weak로 Weak 복사
	template <typename U>
	void WeakChangeToWeak(WeakPointer<U>& weak) {
		SubtractWeakCount();
		m_Pointer = (T*)(weak.m_Pointer);
		m_ControlBlock = weak.m_ControlBlock;
		m_Size = weak.m_Size;
		AddWeakCount();
	}

	// Shared로 Shared 이동
	template <typename U>
	void SharedMoveToShared(SharedPointer<U>& shared) {
		SubtractReferenceCount();

		m_Pointer = (T*)(shared.m_Pointer);
		m_ControlBlock = shared.m_ControlBlock;
		m_Size = shared.m_Size;

		shared.m_Pointer = nullptr;
		shared.m_ControlBlock = nullptr;
		shared.m_Size = 0;
	}

	// Shared로 Weak 이동
	template <typename U>
	void SharedMoveToWeak(WeakPointer<U>& weak) {
		SubtractReferenceCount();
		m_Pointer = weak.m_Pointer;
		m_ControlBlock = weak.m_ControlBlock;

		weak.m_Pointer = nullptr;
		weak.m_ControlBlock = nullptr;
		weak.m_Size = 0;
	}

	template <typename U>
	static void WeakMoveToShared(SharedPointer<U>& shared) {
		// 필요 없음
		DebugAssert(true, "멍미");
	}

	template <typename U>
	void WeakMoveToWeak(WeakPointer<U>& weak) {
		SubtractWeakCount();

		m_Pointer = weak.m_Pointer;
		m_ControlBlock = weak.m_ControlBlock;
		m_ControlBlock = weak.m_Size;

		weak.m_Pointer = nullptr;
		weak.m_ControlBlock = nullptr;
		weak.m_Size = 0;
	}


	void AddReferenceCount() const {
		if (m_ControlBlock == nullptr) {
			return;
		}

		m_ControlBlock->IncreaseRefCount();
	}

	void AddWeakCount() const {
		if (m_ControlBlock == nullptr) {
			return;
		}

		m_ControlBlock->IncreaseWeakCount();
	}

	void SubtractReferenceCount() const {
		if (m_ControlBlock == nullptr) {
			return;
		}

		// 레퍼런스 카운트 줄일때 위크 카운트도 같이 줄이도록 하므로 이 코드 없어야함 ㄷㄷ;
		// 이거땜에 계속 서버 팅겼나보다.. ㅠㅠ
		// 잘못된 메모리참조로 튕길때도 있고 안튕길때도 있어서 찾기가 넘 어려웠다. ㄷㄷ
		// m_ControlBlock->DecreaseWeakCount();
		m_ControlBlock->DecreaseRefCount();
		
	}

	void SubtractWeakCount() const {
		if (m_ControlBlock == nullptr) {
			return;
		}

		m_ControlBlock->DecreaseWeakCount();
	}

	template <typename U>
	void SetSharedPointer(U* ptr, ControlBlock* controlBlock, int size) {

//		SharedPointer<Model[]> p3 = MakeShared<Model2[]>(20); // 모델 객체 20의 배열 생성
// 		배열타입들땜에 강제 형변환 해줘야한다. ㄷㄷ;
// 
//		m_Pointer = static_cast<T*>(ptr);
//		std::cout << Type_v<T> << "\n";
//		std::cout << Type_v<U> << "\n";

		m_Pointer = (T*)(ptr);
		m_ControlBlock = controlBlock;
		m_Size = size;
	}

protected:
	T* m_Pointer = nullptr;
	ControlBlock* m_ControlBlock = nullptr;
	int m_Size = 0;

	template <typename> friend class SharedMaker;
	template <typename> friend class BasePointer;
};


template <typename T>
class SharedPointer : public BasePointer<T>
{
	using TSharedPointer = SharedPointer<T>;
public:
	SharedPointer() {}
	SharedPointer(std::nullptr_t nulptr) {}

	template <typename U>
	SharedPointer(WeakPointer<U>& weak) {
		StaticAssert::CheckDynamicCastable<U, T>();
		this->SharedChangeToWeak(weak);
	}
	
	template <typename U>
	SharedPointer(WeakPointer<U>&& weak) {
		StaticAssert::CheckDynamicCastable<U, T>();
		this->SharedMoveToWeak(weak);
	}

	template <typename U>
	SharedPointer(SharedPointer<U>& shared) {
		StaticAssert::CheckDynamicCastable<U, T>();
		this->SharedChangeToShared(shared);
	}

	template <typename U>
	SharedPointer(SharedPointer<U>&& shared) {
		StaticAssert::CheckDynamicCastable<U, T>();
		this->SharedMoveToShared(shared);
	}

	~SharedPointer() {
		this->SubtractReferenceCount();
	}
	
	TSharedPointer& operator=(std::nullptr_t ptr) {
		this->MakeSharedEmpty();
		return *this;
	}

	template <typename U>
	TSharedPointer& operator=(SharedPointer<U>& other) {
		StaticAssert::CheckDynamicCastable<U, T>();
		this->SharedChangeToShared(other);
		return *this;
	}

	template <typename U>
	TSharedPointer& operator=(SharedPointer<U>&& other) {
		StaticAssert::CheckDynamicCastable<U, T>();
		this->SharedMoveToShared(other);
		return *this;
	}

	template <typename U>
	TSharedPointer& operator=(WeakPointer<U>& other) {
		StaticAssert::CheckDynamicCastable<U, T>();
		this->SharedChangeToWeak(other);
		return *this;
	}

	template <typename U>
	TSharedPointer& operator=(WeakPointer<U>&& other) {
		StaticAssert::CheckDynamicCastable<U, T>();
		this->SharedMoveToWeak(other);
		return *this;
	}
};


template <typename T>
class WeakPointer : public BasePointer<T>
{
	using TWeakPointer = WeakPointer<T>;
public:
	WeakPointer() {}
	WeakPointer(std::nullptr_t nulptr) {}

	template <typename U>
	WeakPointer(WeakPointer<U>& weak) {
		StaticAssert::CheckDynamicCastable<U, T>();
		this->WeakChangeToWeak(weak);
	}

	template <typename U>
	WeakPointer(WeakPointer<U>&& weak) {
		StaticAssert::CheckDynamicCastable<U, T>();
		this->WeakMoveToWeak(weak);
	}

	template <typename U>
	WeakPointer(SharedPointer<U>& shared) {
		StaticAssert::CheckDynamicCastable<U, T>();
		this->WeakChangeToShared(shared);
	}

	// 쉐어드 포인터의 이동은 막도록 하자.
	// 본체가 사라진 녀석을 참조해버릴 수 있으니까
	template <typename U>
	WeakPointer(SharedPointer<U>&& shared) = delete;


	~WeakPointer() {
		this->SubtractWeakCount();
	}

	TWeakPointer& operator=(std::nullptr_t ptr) {
		this->MakeWeakEmpty();
		return *this;
	}


	template <typename U>
	TWeakPointer& operator=(SharedPointer<U>& other) {
		StaticAssert::CheckDynamicCastable<U, T>();
		this->WeakChangeToShared(other);
		return *this;
	}

	template <typename U>
	TWeakPointer& operator=(WeakPointer<U>& other) {
		StaticAssert::CheckDynamicCastable<U, T>();
		this->WeakChangeToWeak(other);
		return *this;
	}

	template <typename U>
	TWeakPointer& operator=(WeakPointer<U>&& other) {
		StaticAssert::CheckDynamicCastable<U, T>();
		this->WeakMoveToWeak(other);
		return *this;
	}

	template <typename U>
	TWeakPointer& operator=(SharedPointer<U>&& other) = delete;
};


template <typename T>
class SharedMaker
{
	static const int ms_uiArraySize = 1;		// 기본적으로 길이는 무조건 1

	using TSharedPointer = SharedPointer<T>;
	using TSharedObject = SharedObject<T>;
public:
	template <typename... Args>
	static constexpr TSharedPointer Create(Args&&... args) {
		TSharedObject* obj = new TSharedObject(Forward<Args>(args)...);
		TSharedPointer sp;
		sp.SetSharedPointer(obj->Address(), obj, ms_uiArraySize);
		return sp;
	}
};

template <typename T, int ArraySize>
class SharedMaker<T[ArraySize]>
{
	using TSharedPointer = SharedPointer<T[ArraySize]>;
	using TSharedObject = SharedObject<T[ArraySize]>;
public:
	template <typename... Args>
	static constexpr TSharedPointer Create(Args&&... args) {
		TSharedObject* obj = new TSharedObject(Forward<Args>(args)...);
		TSharedPointer sp;
		sp.SetSharedPointer(obj->Address(), obj, ArraySize);
		return sp;
	}
};


template <typename T>
class SharedMaker<T[]>
{
	using TSharedPointer = SharedPointer<T[]>;
	using TSharedObject = SharedObject<T[]>;
public:
	template <typename... Args>
	static constexpr TSharedPointer Create(int size, Args&&... args) {
		TSharedObject* obj = new TSharedObject(size, Forward<Args>(args)...);
		TSharedPointer sp;
		sp.SetSharedPointer(obj->Address(), obj, size);
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
bool operator==(const UniquePointer<T>& lhs, const UniquePointer<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const UniquePointer<T>& lhs, const SharedPointer<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const SharedPointer<T>& lhs, const UniquePointer<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const UniquePointer<T>& lhs, const WeakPointer<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const WeakPointer<T>& lhs, const UniquePointer<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const SharedPointer<T>& lhs, const SharedPointer<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const SharedPointer<T>& lhs, const WeakPointer<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const WeakPointer<T>& lhs, const SharedPointer<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const WeakPointer<T>& lhs, const WeakPointer<U>& rhs) {
	return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator==(const UniquePointer<T>& lhs, std::nullptr_t) {
	return lhs.Get() == nullptr;
}

template <typename T, typename U>
bool operator==(std::nullptr_t, const UniquePointer<T>& rhs) {
	return nullptr == rhs.Get();
}

template <typename T, typename U>
bool operator==(const SharedPointer<T>& lhs, std::nullptr_t) {
	return lhs.Get() == nullptr;
}

template <typename T, typename U>
bool operator==(std::nullptr_t, const SharedPointer<T>& rhs) {
	return nullptr == rhs.Get();
}

template <typename T, typename U>
bool operator==(const WeakPointer<T>& lhs, std::nullptr_t) {
	return lhs.Get() == nullptr;
}

template <typename T, typename U>
bool operator==(std::nullptr_t, const WeakPointer<T>& rhs) {
	return nullptr == rhs.Get();
}












} // namespace JCore