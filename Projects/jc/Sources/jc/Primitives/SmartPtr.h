/*
 *	작성자 : 윤정도
 *	스마트 포인터 구현해보기
 *
 *	[현재 구현 사항]
 *	  1. 유니크 포인터
 *	  2. 위크 포인터
 *	  3. 쉐어드 포인터
 *	  4. 모두 배열 기능 지원
 *	  5. Atomic 추가
 *	  6. MakeSharedFromThis 구현
 *	  7. 메모리풀링 지원
 *	=======================================================================================================================
 *
 *	배열 타입까지 구현해버리는 바람에 다이나믹 캐스팅이
 *	실패하는 경우에 대한 처리를 하지 못했다.
 *
 *	코딩을 신경써서 하는 수밖에..
 *	쓰레드 세이프하지 않으므로 세이프 버전을 만들든 해야할 듯
 *
 *	=======================================================================================================================
 *
 *	쓰레드 세이프하게 어떻게 구현해야할까?
 *	UniquePtr은 다른 곳으로 이동할 수가 없으니 상관없고..
 *	SharedPtr은 객체가 생성될때마다 제어블록의 레퍼런스 카운트와 위크 카운트를 수정하게 되니 Atomic 해야된다.
 *
 *	레퍼런스 카운트가 0이되면 내부 객체의 소멸자 호출한다.
 *  SharedPtr끼리만 서로 복사되서 사용하는 객체라면 제어블록만 Atomic하다면 문제가 없을 것이다.
 *  서로다른 쓰레드간에 간섭하는건 제어블록 뿐이기 때문
 *  [상상의 쓰레드 흐름도]
 *
 *
 *	쓰레드1					        | 쓰레드2
 *	struct Foo {};					|
 *	foo = MakeShared<Foo>();		|								// 무조건 하나의 쓰레드에서만 foo가 저장된 데이터 영역에 접근해서 
 *									|								// 생성한다고 가정
 *	Lock							|
 *	SharedPtr<Foo> copy = foo	    | Lock
 *	Unlock                   	    | SharedPtr<Foo> copy = foo     // foo가 저장된 데이터 영역에 대해서 Lock이 필요함.
 *							        | Unlock                        // 이 경우 레퍼런스 카운터, 위크 카운터는 Atomic일 필요가 없다.
 *									|                               // 락때문에 제어블록은 안전하게 복사된다.
 *							        | copy->GetPtr()
 *							        | 
 *	{						        |   {
 *	  SharedPtr<Foo> local1  = copy |      SharedPtr<Foo> local2 = copy;   // 이때 동시에 제어블록에 접근하여 수정하는 경우가 생긴다.
 *	}						        |   }								   // 이런 경우 레퍼런스 카운터, 위크 카운터가 Atomic이어야한다.
 *																		   // 레퍼런스 카운터와 위크 카운터가 Atomic이 보장되면
 *																		   // 생성된 객체의 소멸자 호출과 제어블록의 소멸자 호출이
 *																		   // Atomic하게 수행됨을 보장할 수 있다.
 *																		   // 그리고 레퍼런스 카운트가 0이 되었다는 말은
 *																		   // 더이상 해당 개체에 접근가능한 SharedPtr이 없다는 말과 같다.
 * 
 *	
 *	=======================================================================================================================
 *	UniqueObject<T>			단일 포인터 (동적할당 1회)
 *	UniqueObject<T[Size]>	컴파일타임에 크기가 결정되는 배열 포인터	(동적할당 1회)
 *	UniqueObject<T[]>		런타임에 크기가 결정되는 배열 포인터	(동적할당 2회 UniqueObject 생성할 때 1번, 내부에서 배열 동적할당 할 때 1번)
 *	
 *	SharedObject도 마찬가지로 방식으로 구현함
 */

#pragma once

#include <jc/Declspec.h>
#include <jc/Exception.h>
#include <jc/Deletor.h>
#include <jc/TypeCast.h>
#include <jc/TypeTraits.h>
#include <jc/Assert.h>
#include <jc/Allocator/DefaultAllocator.h>
#include <jc/Primitives/Atomic.h>

NS_JC_BEGIN
template <typename T>
class ObjectPool;

NS_DETAIL_BEGIN
// 스마트포인터는 배열타입은 기본 타입으로 붕괴해서 체크하자.. ㅠ
template <typename Lhs, typename Rhs>
static constexpr bool IsSmartPtrCastable_v = IsDynamicCastable_v<NakedType_t<Lhs>*, NakedType_t<Rhs>*>;

template <typename Lhs, typename Rhs>
static void CheckDynamicCastable()
{
	static_assert(IsSmartPtrCastable_v<Lhs, Rhs>,
	              "... cannot convert! Type T* and U* must be dynamic castable each other");
}

template <typename T>
static void PreventCreatingObjectPoolItem()
{
	static_assert(!IsBaseOf_v<ObjectPool<T>, T>, "... cannot create object pool item using this method");
}

NS_END


// 전방 선언
template <typename, typename> class SharedMaker;
template <typename, typename> class UniqueMaker;
template <typename> class WeakPtr;
template <typename> class SharedPtr;
template <typename> class UniquePtr;
template <typename> class MakeSharedFromThis;
struct MakeSharedFromThisBase;

template <typename T, typename TAllocator = CDefaultAllocator, typename... Args>
constexpr decltype(auto) MakeShared(Args&&... _args)
{
	detail::PreventCreatingObjectPoolItem<NakedType_t<T>>();
	return SharedMaker<T, TAllocator>::Create(Forward<Args>(_args)...);
}

template <typename T, typename TAllocator = CDefaultAllocator, typename... Args>
constexpr decltype(auto) MakeUnique(Args&&... _args)
{
	detail::PreventCreatingObjectPoolItem<NakedType_t<T>>();
	return UniqueMaker<T, TAllocator>::Create(Forward<Args>(_args)...);
}


struct JC_NOVTABLE UniqueBase
{
	virtual ~UniqueBase() = default;
	virtual void DeleteSelf() = 0;
};

template <typename T, typename TAllocator>
struct UniqueObject : UniqueBase
{
	using TDeletor = PlacementDeletor<T, DeletorOption::OnlyDestoryObject>;

	template <typename... Args>
	explicit UniqueObject(Args&&... _args)
	{
		::new(AddressOf(object_)) T(Forward<Args>(_args)...);
	}

	~UniqueObject() override
	{
	}

	void DeleteSelf() override
	{
		TDeletor()(AddressOf(object_));

		TAllocator::template DeallocateStatic<decltype(*this)>(this); // static push
	}

	T* Address() const
	{
		return (T*)AddressOf(object_);
	}

	union
	{
		T object_;
	};
};

template <typename T, int Size, typename TAllocator>
struct UniqueObject<T[Size], TAllocator> : UniqueBase
{
	using TDeletor = PlacementDeletor<T[Size], DeletorOption::OnlyDestoryObject>;

	template <typename... Args>
	explicit UniqueObject(Args&&... _args)
	{
		for (int i = 0; i < Size; i++)
		{
			::new(AddressOf(object_[i])) T{ Forward<Args>(_args)... };
		}
	}

	~UniqueObject() override
	{
	}

	void DeleteSelf() override
	{
		TDeletor()(object_);
		TAllocator::template DeallocateStatic<decltype(*this)>(this);
	}

	T* Address() const
	{
		return (T*)object_;
	}

	union
	{
		T object_[Size];
	};
};

template <typename T, typename TAllocator>
struct UniqueObject<T[], TAllocator> : UniqueBase
{
	using TDeletor = PlacementDeletor<T[], DeletorOption::OnlyDestoryObject>;

	template <typename... Args>
	explicit UniqueObject(int _size, Args&&... _args)
	{
		pointer_ = TAllocator::template AllocateDynamic<T*>(sizeof(T) * _size, allocated_);

		for (int i = 0; i < _size; i++)
		{
			::new(pointer_ + i) T{ Forward<Args>(_args)... };
		}

		size_ = _size;
	}

	~UniqueObject() override
	{
	}

	void DeleteSelf() override
	{
		// Pointer가 가리키는 배열 원소들 명시적으로 소멸자 호출
		TDeletor()(pointer_, size_);
		// Pointer메모리 해제
		TAllocator::DeallocateDynamic(pointer_, allocated_); // dynamic push

		// 이 객체의 메모리를 해제
		TAllocator::template DeallocateStatic<decltype(*this)>(this); // static push
	}

	T* Address() const
	{
		return reinterpret_cast<T*>(pointer_);
	}

	union
	{
		T* pointer_;
	};

	int size_;
	int allocated_;
};

template <typename T>
class UniquePtr
{
	using TUniquePtr = UniquePtr<T>;

	template <typename U>
	void SetUniquePtr(U* _pPtr, UniqueBase* _pBase, int _size)
	{
		pPtr_ = (T*)_pPtr;
		pBase_ = _pBase;
		size_ = _size;
	}

public:
	UniquePtr()
	: pPtr_(nullptr)
	{
	}

	UniquePtr(std::nullptr_t _nulptr)
	: pPtr_(nullptr)
	{
	}

	template <typename U>
	UniquePtr(const UniquePtr<U>& _other) = delete;

	template <typename U>
	UniquePtr(UniquePtr<U>&& _other)
	{
		detail::CheckDynamicCastable<U, T>();

		pPtr_ = (T*)_other.pPtr_;
		pBase_ = _other.pBase_;
		size_ = _other.size_;

		_other.pPtr_ = nullptr;
		_other.pBase_ = nullptr;
		_other.size_ = 0;
	}

	virtual ~UniquePtr()
	{
		if (pBase_ != nullptr)
		{
			// 가상 클래스는 vfptr때문에 스칼라 타입은 4바이트만큼 뒤에 있어서 이렇게 캐스팅해주면 되던데
			// 배열로 만들면 달라지네;
			//UniqueBase* p = (UniqueBase*)((char*)pPtr_ - 4);

			pBase_->DeleteSelf();
			pBase_ = nullptr;
		}
	}

	T& operator*() const
	{
		if (pPtr_ == nullptr)
		{
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}
		return *pPtr_;
	}

	T* operator->() const
	{
		return pPtr_;
	}


	T& operator[](const int _idx) const
	{
		if (pPtr_ == nullptr)
		{
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		if (_idx < 0 || _idx >= size_)
		{
			throw OutOfRangeException("올바른 인덱스 값을 입력해주세요.");
		}

		return pPtr_[_idx];
	}

	int Length() const
	{
		return size_;
	}

	// Get<Model*> -> Model*로 반환
	// 모두 같은 반환을 수행한다.
	template <typename U = T*>
	U Get() const
	{
		if (pPtr_ == nullptr)
		{
			return nullptr;
		}


		static_assert(!IsReferenceType_v<U>, "... cannot cast to reference type"); // Get<int&>와 같은 캐스팅을 방지
		static_assert(IsPointerType_v<U>, "... only cast to pointer type."); // Get<int>	와 같은 방식을 방지

		// U -> int* -> 포인터를 없애주고 비교해야함
		// T -> int
		detail::CheckDynamicCastable<RemovePointer_t<U>, T>();
		return (U)pPtr_;
	}

	template <typename AnyType>
	void operator=(AnyType&& _other) = delete;

private:
	T* pPtr_ = nullptr;
	UniqueBase* pBase_ = nullptr;
	int size_ = 0;

	template <typename, typename> friend class UniqueMaker;
	template <typename> friend class UniquePtr;

	// template <typename U> friend bool operator==(const TUniquePtr&,		const UniquePtr<U>&);
	// template <typename U> friend bool operator==(const UniquePtr<U>&,	const TUniquePtr&);
	// template <typename U> friend bool operator!=(const TUniquePtr&,		const UniquePtr<U>&);
	// template <typename U> friend bool operator!=(const UniquePtr<U>&,	const TUniquePtr&);
};


template <typename T, typename TAllocator>
class UniqueMaker
{
	static constexpr int ARRAY_SIZE = 1; // 기본적으로 길이는 무조건 1

	using TUniquePtr = UniquePtr<T>;
	using TUniqueObject = UniqueObject<T, TAllocator>;

public:
	template <typename... Args>
	static constexpr TUniquePtr Create(Args&&... _args)
	{
		auto obj = TAllocator::template AllocateStatic<TUniqueObject>();
		Memory::PlacementNew(obj, Forward<Args>(_args)...);
		TUniquePtr sp;
		sp.SetUniquePtr(obj->Address(), obj, ARRAY_SIZE);
		return sp;
	}
};

template <typename T, int ArraySize, typename TAllocator>
class UniqueMaker<T[ArraySize], TAllocator>
{
	using TUniquePtr = UniquePtr<T[ArraySize]>;
	using TUniqueObject = UniqueObject<T[ArraySize], TAllocator>;

public:
	template <typename... Args>
	static constexpr TUniquePtr Create(Args&&... _args)
	{
		auto obj = TAllocator::template AllocateStatic<TUniqueObject>();
		Memory::PlacementNew(obj, Forward<Args>(_args)...);
		TUniquePtr sp;
		sp.SetUniquePtr(obj->Address(), obj, ArraySize);
		return sp;
	}
};


template <typename T, typename TAllocator>
class UniqueMaker<T[], TAllocator>
{
	using TUniquePtr = UniquePtr<T[]>;
	using TUniqueObject = UniqueObject<T[], TAllocator>;

public:
	template <typename... Args>
	static constexpr TUniquePtr Create(int _size, Args&&... _args)
	{
		auto obj = TAllocator::template AllocateStatic<TUniqueObject>();
		Memory::PlacementNew(obj, _size, Forward<Args>(_args)...);
		TUniquePtr sp;
		sp.SetUniquePtr(obj->Address(), obj, _size);
		return sp;
	}
};


struct JC_NOVTABLE ControlBlock
{
	ControlBlock() = default;
	virtual ~ControlBlock() = default;

	virtual void DeleteSelf() = 0;
	virtual void DestroyObject() = 0;

	// 레퍼런스 카운트 올릴 때 위크 카운트도 같이 올려준다.
	void IncreaseRefCount()
	{
		++referenceCount_;
		++weakCount_;
	}

	void IncreaseWeakCount()
	{
		++weakCount_;
	}

	void DecreaseRefCount()
	{
		if ((--referenceCount_) == 0)
			DestroyObject();
		if ((--weakCount_) == 0)
			DeleteSelf();
	}

	void DecreaseWeakCount()
	{
		if ((--weakCount_) == 0)
		{
			DeleteSelf();
		}
	}


	Atomic<int> referenceCount_ = 1;
	Atomic<int> weakCount_ = 1;
};


template <typename T, typename TAllocator>
struct SharedObject : ControlBlock
{
	using TDeletor = PlacementDeletor<T, DeletorOption::OnlyDestoryObject>;

	template <typename... Args>
	explicit SharedObject(Args&&... args)
	{
		::new(AddressOf(object_)) T(Forward<Args>(args)...);
	}

	~SharedObject() override
	{
	}

	void DestroyObject() override
	{
		TDeletor()(AddressOf(object_));
	}

	void DeleteSelf() override
	{
		TAllocator::template DeallocateStatic<decltype(*this)>(this);
	}

	T* Address() const
	{
		return (T*)AddressOf(object_);
	}

	union
	{
		T object_;
	};
};


// 외부에서 직접 포인터를 넣어주는 경우때문에 추가해줌
template <typename T, typename TAllocator>
struct SharedObject<T*, TAllocator> : ControlBlock
{
	using TDeletor = PlacementDeletor<T, DeletorOption::Both>;

	explicit SharedObject(T* _ptr)
	{
		object_ = _ptr;
	}

	~SharedObject() override
	{
	}

	void DestroyObject() override
	{
		if constexpr (IsBaseOf_v<ObjectPool<T>, T>)
		{
			ObjectPool<T>::PushObjectWithDestroy(object_);
		}
		else
		{
			TDeletor()(object_);
		}
	}

	void DeleteSelf() override
	{
		TAllocator::template DeallocateStatic<decltype(*this)>(this);
	}

	T* Address() const
	{
		return object_;
	}

	T* object_;
};

template <typename T, int Size, typename TAllocator>
struct SharedObject<T[Size], TAllocator> : ControlBlock
{
	using TDeletor = PlacementDeletor<T[Size], DeletorOption::OnlyDestoryObject>;

	template <typename... Args>
	explicit SharedObject(Args&&... args)
	{
		for (int i = 0; i < Size; i++)
		{
			::new(AddressOf(object_[i])) T(Forward<Args>(args)...);
		}
	}

	~SharedObject() override
	{
	}

	void DestroyObject() override
	{
		TDeletor()(object_);
	}

	void DeleteSelf() override
	{
		TAllocator::template DeallocateStatic<decltype(*this)>(this);
	}

	T* Address() const
	{
		return (T*)object_;
	}

	union
	{
		T object_[Size];
	};
};

template <typename T, typename TAllocator>
struct SharedObject<T[], TAllocator> : ControlBlock
{
	using TDeletor = PlacementDeletor<T[], DeletorOption::OnlyDestoryObject>;

	template <typename... Args>
	explicit SharedObject(int _size, Args&&... _args)
	{
		pointer_ = TAllocator::template AllocateDynamic<T*>(sizeof(T) * _size, allocated_);

		for (int i = 0; i < _size; i++)
		{
			::new(pointer_ + i) T{ Forward<Args>(_args)... };
		}

		size_ = _size;
	}

	~SharedObject() override
	{
	}

	void DestroyObject() override
	{
		TDeletor()(pointer_, size_);
	}

	void DeleteSelf() override
	{
		// Pointer가 가리키는 배열 원소들 명시적으로 소멸자 호출
		TDeletor()(pointer_, size_);
		// Pointer메모리 해제
		TAllocator::DeallocateDynamic(pointer_, allocated_); // dynamic push

		// 이 객체의 메모리를 해제
		TAllocator::template DeallocateStatic<decltype(*this)>(this); // static push
	}

	T* Address() const
	{
		return reinterpret_cast<T*>(pointer_);
	}

	union
	{
		T* pointer_;
	};

	int size_;
	int allocated_;
};


template <typename T>
class BasePtr
{
public:
	using TType = T;
	using TSharedPtr = SharedPtr<T>;
	using TWeakPtr = WeakPtr<T>;
	using TPortable = RemoveArray_t<T>; // T[], T[20] -> T

	int RefCount() const
	{
		if (pControlBlock_ == nullptr)
		{
			return 0;
		}

		return pControlBlock_->referenceCount_;
	}

	int WeakCount() const
	{
		if (pControlBlock_ == nullptr)
		{
			return 0;
		}

		return pControlBlock_->weakCount_;
	}


	bool Exist() const
	{
		if (pControlBlock_ == nullptr)
			return false;

		return pControlBlock_->referenceCount_ > 0;
	}

	T& GetRef() const
	{
		return *pPtr_;
	}

	TPortable* GetPtr() const
	{
		return pPtr_;
	}

	int Length() const
	{
		return size_;
	}

	// Get<Model*> -> Model*로 반환
	// 모두 같은 반환을 수행한다.
	template <typename U = T*>
	U Get() const
	{
		if (!Exist())
		{
			return nullptr;
		}

		static_assert(!IsReferenceType_v<U>, "... cannot cast to reference type"); // Get<int&>와 같은 캐스팅을 방지
		static_assert(IsPointerType_v<U>, "... only cast to pointer type."); // Get<int>	와 같은 방식을 방지

		// U -> int* -> 포인터를 없애주고 비교해야함
		// T -> int
		detail::CheckDynamicCastable<RemovePointer_t<U>, T>();
		return (U)pPtr_;
	}

	T& operator*() const
	{
		if (!Exist())
		{
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return GetRef();
	}

	T* operator->() const
	{
		if (!Exist())
		{
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return GetPtr();
	}

	T& operator[](const int _idx) const
	{
		if (!Exist())
		{
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		if (_idx < 0 || _idx >= size_)
		{
			throw OutOfRangeException("올바른 인덱스 값을 입력해주세요.");
		}

		return pPtr_[_idx];
	}

protected:
	void MakeSharedEmpty()
	{
		if (pControlBlock_ != nullptr)
			pControlBlock_->DecreaseRefCount();

		pPtr_ = nullptr;
		pControlBlock_ = nullptr;
		size_ = 0;
	}

	// SharedPtr에서만 호출
	template <typename U, DefaultEnableIf_t<IsPointerType_v<U>> = nullptr>
	void MakeShared(U _ptr);

	// Shared로 Shared 복사
	template <typename U>
	void SharedChangeToShared(SharedPtr<U>& _shared)
	{
		SubtractReferenceCount();
		pPtr_ = (TPortable*)_shared.pPtr_;
		pControlBlock_ = _shared.pControlBlock_;
		size_ = _shared.size_;
		AddReferenceCount();
	}

	// Shared로 Weak 복사
	template <typename U>
	void SharedChangeToWeak(WeakPtr<U>& _weak)
	{
		SubtractReferenceCount();
		pPtr_ = (TPortable*)_weak.pPtr_;
		pControlBlock_ = _weak.pControlBlock_;
		size_ = _weak.size_;
		AddReferenceCount();
	}

	// Weak로 Shared 복사
	template <typename U>
	void WeakChangeToShared(SharedPtr<U>& _shared)
	{
		SubtractWeakCount();
		pPtr_ = (TPortable*)_shared.pPtr_;
		pControlBlock_ = _shared.pControlBlock_;
		size_ = _shared.size_;
		AddWeakCount();
	}

	// Weak로 Weak 복사
	template <typename U>
	void WeakChangeToWeak(WeakPtr<U>& _weak)
	{
		SubtractWeakCount();
		pPtr_ = (TPortable*)_weak.pPtr_;
		pControlBlock_ = _weak.pControlBlock_;
		size_ = _weak.size_;
		AddWeakCount();
	}

	// Shared로 Shared 이동
	template <typename U>
	void SharedMoveToShared(SharedPtr<U>& _shared)
	{
		SubtractReferenceCount();

		pPtr_ = (TPortable*)_shared.pPtr_;
		pControlBlock_ = _shared.pControlBlock_;
		size_ = _shared.size_;

		_shared.pPtr_ = nullptr;
		_shared.pControlBlock_ = nullptr;
		_shared.size_ = 0;
	}

	// Shared로 Weak 이동
	template <typename U>
	void SharedMoveToWeak(WeakPtr<U>& _weak)
	{
		SubtractReferenceCount();
		pPtr_ = _weak.pPtr_;
		pControlBlock_ = _weak.pControlBlock_;

		_weak.pPtr_ = nullptr;
		_weak.pControlBlock_ = nullptr;
		_weak.size_ = 0;
	}

	template <typename U>
	static void WeakMoveToShared(SharedPtr<U>& _shared)
	{
		// 필요 없음
		jc_assert_msg(true, "멍미");
	}

	template <typename U>
	void WeakMoveToWeak(WeakPtr<U>& _weak)
	{
		SubtractWeakCount();

		pPtr_ = _weak.pPtr_;
		pControlBlock_ = _weak.pControlBlock_;
		size_ = _weak.size_;

		_weak.pPtr_ = nullptr;
		_weak.pControlBlock_ = nullptr;
		_weak.size_ = 0;
	}


	void AddReferenceCount() const
	{
		if (pControlBlock_ == nullptr)
		{
			return;
		}

		pControlBlock_->IncreaseRefCount();
	}

	void AddWeakCount() const
	{
		if (pControlBlock_ == nullptr)
		{
			return;
		}

		pControlBlock_->IncreaseWeakCount();
	}

	void SubtractReferenceCount() const
	{
		if (pControlBlock_ == nullptr)
		{
			return;
		}

		// 레퍼런스 카운트 줄일때 위크 카운트도 같이 줄이도록 하므로 이 코드 없어야함 ㄷㄷ;
		// 이거땜에 계속 서버 팅겼나보다.. ㅠㅠ
		// 잘못된 메모리참조로 튕길때도 있고 안튕길때도 있어서 찾기가 넘 어려웠다. ㄷㄷ
		// m_ControlBlock->DecreaseWeakCount();
		pControlBlock_->DecreaseRefCount();
	}

	void SubtractWeakCount() const
	{
		if (pControlBlock_ == nullptr)
		{
			return;
		}

		pControlBlock_->DecreaseWeakCount();
	}

	void SetSharedPtr(TPortable* _ptr, ControlBlock* _controlBlock, int _size)
	{
		// SharedPtr<IRunnable> sp = MakeShared<Runnable[20]>();		// 컴파일타임 크기 결정
		// SetSharedPtr<MyThread[20]>(MyThread*, ControlBlock*, int);
		// SharedPtr<IRunnable> sp = MakeShared<Runnable[]>(20);		// 런타임 크기 결정
		// SetSharedPtr<MyThread[0]>(MyThread*, ControlBlock*, int);

		pPtr_ = _ptr;
		pControlBlock_ = _controlBlock;
		size_ = _size;

		// MakeSharedFromThis를 상속받지 않은 경우 그만
		// if constexpr 특성상 if else 분리가 불가능함 ㅠ
		if constexpr (IsBaseOf_v<MakeSharedFromThisBase, TPortable>)
		{
			// 여기 들어오는 타입은 무조건 SharedPtr타입이므로 강제 케스팅 해줌
			TSharedPtr& thisRef = static_cast<TSharedPtr&>(*this);

			// 배열 타입이 아닌 경우 그냥 대입만하고 끝
			if constexpr (IsArrayType_v<T>)
			{
				for (int i = 0; i < _size; ++i)
				{
					_ptr[i].m_spWeak = thisRef;
				}
			}
			else
			{
				_ptr->m_spWeak = thisRef;
			}
		}
	}

protected:
	TPortable* pPtr_ = nullptr;
	ControlBlock* pControlBlock_ = nullptr;
	int size_ = 0;

	template <typename, typename> friend class SharedMaker;
	template <typename> friend class BasePtr;
};

template <typename T> template <typename U, DefaultEnableIf_t<IsPointerType_v<U>>> void BasePtr<T>::MakeShared(U _ptr)
{
	pControlBlock_ = dbg_new SharedObject<U, CDefaultAllocator>(_ptr);
	pPtr_ = _ptr;
	size_ = 1;

	if constexpr (IsBaseOf_v<MakeSharedFromThis<NakedType_t<U>>, NakedType_t<U>>)
		_ptr->m_spWeak = *static_cast<SharedPtr<T>*>(this);
}


template <typename T>
class SharedPtr : public BasePtr<T>
{
	using TSharedPtr = SharedPtr<T>;

public:
	SharedPtr()
	{
	}

	SharedPtr(std::nullptr_t)
	{
	}

	// 외부에서 포인터를 직접 넣어주는 경우
	// 밖에서 포인터 생성하고 안에서 또 제어블록을 생성해서 비효율적이지만
	// 생성자가 private으로 선언된 경우 이 방식으로 초기화해야함
	// 
	// ┌ SharedPtr<int[20]>(new int);  ==> 지원안함
	// │ SharedPtr<int[]>(new int[20]);==> 지원안함
	// └ SharedPtr<int>(new int);      ==> 이거만 가능
	//
	// 진짜 라이브러리 만드는게 쉬운게 아니구나;; 혼자서는 이런거 하나하나 다 캐치해낼 수가 없다.

	// SharedPtr(T* ptr)를 쓰지 않고 굳이 U로 받은 이유는
	// 밖에서 전달해준 실체 타입이 필요하기 때문이다.
	// struct IRunnable {}
	// struct Thread: MakeSharedFromThis, IRunnable
	// SharedPtr<IRunnable>(new Thread) -> 이렇게 해버릴 수 있음
	// 따라서 IRunnable로는 MakeSharedFromThis를 상속받고 있는지 알 수가 없다.
	template <typename U, DefaultEnableIf_t<IsPointerType_v<U>> = nullptr>
	SharedPtr(U _ptr)
	{
		detail::CheckDynamicCastable<U, T>();
		this->MakeShared(_ptr);
	}

	template <typename U>
	SharedPtr(WeakPtr<U>& _weak)
	{
		detail::CheckDynamicCastable<U, T>();
		this->SharedChangeToWeak(_weak);
	}

	template <typename U>
	SharedPtr(WeakPtr<U>&& _weak)
	{
		detail::CheckDynamicCastable<U, T>();
		this->SharedMoveToWeak(_weak);
	}

	SharedPtr(const TSharedPtr& _other)
	{
		this->SharedChangeToShared(const_cast<TSharedPtr&>(_other));
	}

	SharedPtr(TSharedPtr&& _other)
	{
		this->SharedMoveToShared(_other);
	}

	template <typename U>
	SharedPtr(const SharedPtr<U>& _shared)
	{
		detail::CheckDynamicCastable<U, T>();
		this->SharedChangeToShared(const_cast<SharedPtr<U>&>(_shared));
	}

	template <typename U>
	SharedPtr(SharedPtr<U>&& _shared)
	{
		detail::CheckDynamicCastable<U, T>();
		this->SharedMoveToShared(_shared);
	}

	~SharedPtr()
	{
		this->SubtractReferenceCount();
	}

	TSharedPtr& operator=(std::nullptr_t _ptr)
	{
		this->MakeSharedEmpty();
		return *this;
	}

	TSharedPtr& operator=(const TSharedPtr& _other)
	{
		this->SharedChangeToShared(const_cast<TSharedPtr&>(_other));
		return *this;
	}

	TSharedPtr& operator=(TSharedPtr&& _other) noexcept
	{
		this->SharedChangeToShared(_other);
		return *this;
	}

	template <typename U>
	TSharedPtr& operator=(const SharedPtr<U>& _other)
	{
		detail::CheckDynamicCastable<U, T>();
		this->SharedChangeToShared(const_cast<SharedPtr<U>&>(_other));
		return *this;
	}

	template <typename U>
	TSharedPtr& operator=(SharedPtr<U>&& _other)
	{
		detail::CheckDynamicCastable<U, T>();
		this->SharedMoveToShared(_other);
		return *this;
	}

	template <typename U>
	TSharedPtr& operator=(WeakPtr<U>& _other)
	{
		detail::CheckDynamicCastable<U, T>();
		this->SharedChangeToWeak(_other);
		return *this;
	}


	template <typename U>
	TSharedPtr& operator=(WeakPtr<U>&& _other)
	{
		detail::CheckDynamicCastable<U, T>();
		this->SharedMoveToWeak(_other);
		return *this;
	}

	// template <typename U> friend bool operator==(const TSharedPtr&,		const SharedPtr<U>&);
	// template <typename U> friend bool operator==(const SharedPtr<U>&,	const TSharedPtr&);
	// template <typename U> friend bool operator!=(const TSharedPtr&,		const SharedPtr<U>&);
	// template <typename U> friend bool operator!=(const SharedPtr<U>&,	const TSharedPtr&);
};


template <typename T>
class WeakPtr : public BasePtr<T>
{
	using TSharedPtr = SharedPtr<T>;
	using TWeakPtr = WeakPtr<T>;

public:
	WeakPtr()
	{
	}

	WeakPtr(std::nullptr_t)
	{
	}


	WeakPtr(const TWeakPtr& _other)
	{
		this->WeakChangeToWeak(const_cast<TWeakPtr&>(_other));
	}

	WeakPtr(TWeakPtr&& _other) noexcept
	{
		this->WeakMoveToWeak(_other);
	}

	template <typename U>
	WeakPtr(const WeakPtr<U>& _weak)
	{
		detail::CheckDynamicCastable<U, T>();
		this->WeakChangeToWeak(const_cast<TWeakPtr&>(_weak));
	}

	template <typename U>
	WeakPtr(WeakPtr<U>&& _weak)
	{
		detail::CheckDynamicCastable<U, T>();
		this->WeakMoveToWeak(_weak);
	}

	template <typename U>
	WeakPtr(const SharedPtr<U>& _shared)
	{
		detail::CheckDynamicCastable<U, T>();
		this->WeakChangeToShared(const_cast<SharedPtr<U>&>(_shared));
	}

	// 쉐어드 포인터의 이동은 막도록 하자.
	// 본체가 사라진 녀석을 참조해버릴 수 있으니까
	template <typename U>
	WeakPtr(SharedPtr<U>&& _shared) = delete;
	WeakPtr(TSharedPtr&& _other) = delete;


	~WeakPtr()
	{
		this->SubtractWeakCount();
	}

	TWeakPtr& operator=(std::nullptr_t)
	{
		this->MakeWeakEmpty();
		return *this;
	}


	template <typename U>
	TWeakPtr& operator=(SharedPtr<U>& _other)
	{
		detail::CheckDynamicCastable<U, T>();
		this->WeakChangeToShared(_other);
		return *this;
	}

	template <typename U>
	TWeakPtr& operator=(WeakPtr<U>& _other)
	{
		detail::CheckDynamicCastable<U, T>();
		this->WeakChangeToWeak(_other);
		return *this;
	}

	template <typename U>
	TWeakPtr& operator=(WeakPtr<U>&& _other)
	{
		detail::CheckDynamicCastable<U, T>();
		this->WeakMoveToWeak(_other);
		return *this;
	}

	template <typename U>
	TWeakPtr& operator=(SharedPtr<U>&& _other) = delete;

	// template <typename U> friend bool operator==(const TWeakPtr&,		const WeakPtr<U>&);
	// template <typename U> friend bool operator==(const WeakPtr<U>&,		const TWeakPtr&);
	// template <typename U> friend bool operator!=(const TWeakPtr&,		const WeakPtr<U>&);
	// template <typename U> friend bool operator!=(const WeakPtr<U>&,		const TWeakPtr&);
};


template <typename T, typename TAllocator>
class SharedMaker
{
	static constexpr int ARRAY_SIZE = 1; // 기본적으로 길이는 무조건 1

	using TSharedPtr = SharedPtr<T>;
	using TSharedObject = SharedObject<T, TAllocator>;

public:
	template <typename... Args>
	static constexpr TSharedPtr Create(Args&&... _args)
	{
		auto obj = TAllocator::template AllocateStatic<TSharedObject>();
		Memory::PlacementNew(obj, Forward<Args>(_args)...);
		TSharedPtr sp;
		sp.SetSharedPtr(obj->Address(), obj, ARRAY_SIZE);
		return sp;
	}
};

template <typename T, int ArraySize, typename TAllocator>
class SharedMaker<T[ArraySize], TAllocator>
{
	using TSharedPtr = SharedPtr<T[ArraySize]>;
	using TSharedObject = SharedObject<T[ArraySize], TAllocator>;

public:
	template <typename... Args>
	static constexpr TSharedPtr Create(Args&&... _args)
	{
		auto obj = TAllocator::template AllocateStatic<TSharedObject>();
		Memory::PlacementNew(obj, Forward<Args>(_args)...);
		TSharedPtr sp;
		sp.SetSharedPtr(obj->Address(), obj, ArraySize);
		return sp;
	}
};


template <typename T, typename TAllocator>
class SharedMaker<T[], TAllocator>
{
	using TSharedPtr = SharedPtr<T[]>;
	using TSharedObject = SharedObject<T[], TAllocator>;

public:
	template <typename... Args>
	static constexpr TSharedPtr Create(int _size, Args&&... _args)
	{
		auto obj = TAllocator::template AllocateStatic<TSharedObject>();
		Memory::PlacementNew(obj, _size, Forward<Args>(_args)...);
		TSharedPtr sp;
		sp.SetSharedPtr(obj->Address(), obj, _size);
		return sp;
	}
};

// 기존 C++ 표준 라이브러리 분석 후 내 라이브러리에도 적용
// 엄청 편리한 기능인것같다.
// 템플릿 제거용
struct MakeSharedFromThisBase
{
};

template <typename T>
class MakeSharedFromThis : MakeSharedFromThisBase
{
	using TSharedPtr = SharedPtr<T>;
	using TWeakPtr = WeakPtr<T>;

public:
	TSharedPtr Shared() { return m_spWeak; }
	TWeakPtr Weak() { return m_spWeak; }
	int RefCount() const { return m_spWeak.RefCount(); }
	int WeakCount() const { return m_spWeak.WeakCount(); }

protected:
	MakeSharedFromThis()
	: m_spWeak()
	{
	}

	TWeakPtr m_spWeak;
	template <typename> friend class BasePtr;
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
bool operator==(const UniquePtr<T>& _lhs, const UniquePtr<U>& _rhs)
{
	return _lhs.Get() == _rhs.Get();
}

template <typename T, typename U>
bool operator==(const UniquePtr<T>& _lhs, const SharedPtr<U>& _rhs)
{
	return _lhs.Get() == _rhs.Get();
}

template <typename T, typename U>
bool operator==(const SharedPtr<T>& _lhs, const UniquePtr<U>& _rhs)
{
	return _lhs.Get() == _rhs.Get();
}

template <typename T, typename U>
bool operator==(const UniquePtr<T>& _lhs, const WeakPtr<U>& _rhs)
{
	return _lhs.Get() == _rhs.Get();
}

template <typename T, typename U>
bool operator==(const WeakPtr<T>& _lhs, const UniquePtr<U>& _rhs)
{
	return _lhs.Get() == _rhs.Get();
}

template <typename T, typename U>
bool operator==(const SharedPtr<T>& _lhs, const SharedPtr<U>& _rhs)
{
	return _lhs.Get() == _rhs.Get();
}

template <typename T, typename U>
bool operator==(const SharedPtr<T>& _lhs, const WeakPtr<U>& _rhs)
{
	return _lhs.Get() == _rhs.Get();
}

template <typename T, typename U>
bool operator==(const WeakPtr<T>& _lhs, const SharedPtr<U>& _rhs)
{
	return _lhs.Get() == _rhs.Get();
}

template <typename T, typename U>
bool operator==(const WeakPtr<T>& _lhs, const WeakPtr<U>& _rhs)
{
	return _lhs.Get() == _rhs.Get();
}

template <typename T>
bool operator==(const UniquePtr<T>& _lhs, std::nullptr_t)
{
	return _lhs.Get() == nullptr;
}

template <typename T>
bool operator==(std::nullptr_t, const UniquePtr<T>& _rhs)
{
	return nullptr == _rhs.Get();
}

template <typename T>
bool operator==(const SharedPtr<T>& _lhs, std::nullptr_t)
{
	return _lhs.Get() == nullptr;
}

template <typename T>
bool operator==(std::nullptr_t, const SharedPtr<T>& _rhs)
{
	return nullptr == _rhs.Get();
}

template <typename T>
bool operator==(const WeakPtr<T>& _lhs, std::nullptr_t)
{
	return _lhs.Get() == nullptr;
}

template <typename T>
bool operator==(std::nullptr_t, const WeakPtr<T>& _rhs)
{
	return nullptr == _rhs.Get();
}


template <typename T, typename U>
bool operator!=(const UniquePtr<T>& _lhs, const UniquePtr<U>& _rhs)
{
	return _lhs.Get() != _rhs.Get();
}

template <typename T, typename U>
bool operator!=(const UniquePtr<T>& _lhs, const SharedPtr<U>& _rhs)
{
	return _lhs.Get() != _rhs.Get();
}

template <typename T, typename U>
bool operator!=(const SharedPtr<T>& _lhs, const UniquePtr<U>& _rhs)
{
	return _lhs.Get() != _rhs.Get();
}

template <typename T, typename U>
bool operator!=(const UniquePtr<T>& _lhs, const WeakPtr<U>& _rhs)
{
	return _lhs.Get() != _rhs.Get();
}

template <typename T, typename U>
bool operator!=(const WeakPtr<T>& _lhs, const UniquePtr<U>& _rhs)
{
	return _lhs.Get() != _rhs.Get();
}

template <typename T, typename U>
bool operator!=(const SharedPtr<T>& _lhs, const SharedPtr<U>& _rhs)
{
	return _lhs.Get() != _rhs.Get();
}

template <typename T, typename U>
bool operator!=(const SharedPtr<T>& _lhs, const WeakPtr<U>& _rhs)
{
	return _lhs.Get() != _rhs.Get();
}

template <typename T, typename U>
bool operator!=(const WeakPtr<T>& _lhs, const SharedPtr<U>& _rhs)
{
	return _lhs.Get() != _rhs.Get();
}

template <typename T, typename U>
bool operator!=(const WeakPtr<T>& _lhs, const WeakPtr<U>& _rhs)
{
	return _lhs.Get() != _rhs.Get();
}

template <typename T>
bool operator!=(const UniquePtr<T>& _lhs, std::nullptr_t)
{
	return _lhs.Get() != nullptr;
}

template <typename T>
bool operator!=(std::nullptr_t, const UniquePtr<T>& _rhs)
{
	return nullptr != _rhs.Get();
}

template <typename T>
bool operator!=(const SharedPtr<T>& _lhs, std::nullptr_t)
{
	return _lhs.Get() != nullptr;
}

template <typename T>
bool operator!=(std::nullptr_t, const SharedPtr<T>& _rhs)
{
	return nullptr != _rhs.Get();
}

template <typename T>
bool operator!=(const WeakPtr<T>& _lhs, std::nullptr_t)
{
	return _lhs.Get() != nullptr;
}

template <typename T>
bool operator!=(std::nullptr_t, const WeakPtr<T>& _rhs)
{
	return nullptr != _rhs.Get();
}


NS_END
