/*
	작성자 : 윤정도
	해당 포인터가 살아있는지 여부를 알 수 있는 스마트 포인터, 쓰레드 세이프 하지 않음

	Owner : 포인터의 주인 => Owner 객체간에는 포인터 복사가 불가능하다.
	Watcher : 주인을 감시하는 놈 => 오너의 포인터 정보를 복사하여 접근하여 사용 가능
*/


#pragma once

#include <JCore/Declspec.h>
#include <JCore/Exception.h>
#include <JCore/TypeTraits.h>
#include <JCore/Assert.h>
#include <JCore/Define.h>
#include <JCore/Debug/New.h>

NS_JC_BEGIN

NS_DETAIL_BEGIN
template <typename T, typename U>
constexpr bool IsStaticCastable() {
	return IsFundamentalType_v<T> && IsFundamentalType_v<U>;
}

template <typename U, typename T>
constexpr bool IsDynamicCastable() {
	return IsBaseOf_v<T, U> || IsBaseOf_v<U, T>;
}
NS_DETAIL_END

class VoidOwner;
class VoidWatcher;

struct VoidPointerCounter {
	bool Alive = true;
	int Counter = 1;
};

/*=====================================================================================
									Void 포인터 베이스
 Void 왓쳐와 Void 오너의 기능을 포함하고 있는 객체
 =====================================================================================*/
class VoidBase {
public:
	VoidBase() = default;
	virtual ~VoidBase() noexcept = default;

	template <typename T, DefaultEnableIf_t<IsPointerType_v<T>> = nullptr>
	T Get() const {
		return (T)m_pPointer;
	}

	void* GetRaw() const {
		return m_pPointer;
	}


	bool Exist() const {
		if (m_pPointer == nullptr || m_pCounter == nullptr) {
			return false;
		}

		return m_pCounter->Alive;
	}

	int WatcherCount() const {
		if (m_pCounter == nullptr) {
			return false;
		}

		return m_pCounter->Counter;
	}

protected:
	void DeletePointer() {
		if (m_pPointer == nullptr) {
			return;
		}

		if (!m_bNoDelete) {
			JCORE_DELETE_SAFE(m_pPointer);
		}

		m_pPointer = nullptr;
		m_pCounter->Alive = false;
		m_pCounter->Counter--;

		if (m_pCounter->Counter == 0) {
			JCORE_DELETE_SAFE(m_pCounter);
		}
	}

	void SubtractWatcherCount() {
		if (m_pCounter == nullptr) {
			return;
		}

		m_pCounter->Counter--;
		if (m_pCounter->Counter == 0) {
			JCORE_DELETE_SAFE(m_pCounter);
		}
	}

	void AddWatcherCount() const {
		if (m_pCounter == nullptr) {
			return;
		}

		m_pCounter->Counter++;
	}

	void OwnerMoveToOwner(VoidOwner& owner);
	void WatcherCopyToOwner(const VoidOwner& owner);
	void WatcherCopyToWatcher(const VoidWatcher& watcher);
	void WatcherMoveToWatcher(VoidWatcher& watcher);
protected:
	bool m_bNoDelete = false;
	void* m_pPointer = nullptr;
	VoidPointerCounter* m_pCounter = nullptr;
};




/*=====================================================================================
									Void 오너
 포인터 소유자로 이녀석이 해제되면 이 포인터의 감시자들이 모두 무효화된다.
 ControlBlock으로 이를 인지할 수 있다.
 =====================================================================================*/
class VoidOwner : public VoidBase 
{
	friend class VoidWatcher;
public:
	VoidOwner(void* ptr, bool nodelete = false) {
		m_pPointer = ptr;
		m_pCounter = dbg_new VoidPointerCounter();
		m_bNoDelete = nodelete;
	}

	VoidOwner(const VoidOwner&) = delete;
	VoidOwner(VoidOwner&& owner) noexcept {
		OwnerMoveToOwner(owner);
	}

	~VoidOwner() override {
		DeletePointer();
	}

	bool Free() {
		if (m_pPointer == nullptr) {
			return false;
		}

		DeletePointer();
		return true;
	}

	void operator=(const VoidOwner&) = delete;
	void operator=(VoidOwner&& owner) noexcept {
		OwnerMoveToOwner(owner);
	}
};



/*=====================================================================================
									Void 포인터 감시자
 =====================================================================================*/

class VoidWatcher : public VoidBase
{
	friend class VoidOwner;
public:
	VoidWatcher() = default;

	VoidWatcher(VoidOwner& owner) {
		WatcherCopyToOwner(owner);
	}

	VoidWatcher(VoidWatcher& watcher) {
		WatcherCopyToWatcher(watcher);
	}

	VoidWatcher(VoidWatcher&& watcher) noexcept {
		WatcherMoveToWatcher(watcher);
	}

	VoidWatcher(VoidOwner&& monitor) = delete;

	~VoidWatcher() override {
		SubtractWatcherCount();
	}

	VoidWatcher& operator=(VoidOwner& owner) {
		WatcherCopyToOwner(owner);
		return *this;
	}

	VoidWatcher& operator=(VoidOwner&& owner) = delete;

	VoidWatcher& operator=(VoidWatcher& watcher) {
		WatcherCopyToWatcher(watcher);
		return *this;
	}

	VoidWatcher& operator=(VoidWatcher&& watcher) noexcept {
		WatcherMoveToWatcher(watcher);
		return *this;
	}
};



/*=====================================================================================
								

								Void 오너/왓쳐의 템플릿 버전입니다.


 =====================================================================================*/


struct JCORE_NOVTABLE PtrCounter
{
	virtual void DestroyObject() = 0;

	bool Alive = true;
	int Counter = 1;
};


template <typename> struct OwnerObject;
template <typename> class Owner;
template <typename> class Watcher;


template <typename T>
struct OwnerObject : PtrCounter
{
	template <typename... Args>
	explicit OwnerObject(Args&&... args) {
		::new (AddressOf(Object)) T{ Forward<Args>(args)... };
	}

	void DestroyObject() override {
		Object.~T();
	}

	T* Address() const {
		return (T*)AddressOf(Object);
	}

	union { T Object; };	// 디폴트 생성자 호출 방지
};



template <typename T>
class Base
{
	using TBase				= Base<T>;
	using TOwner			= Owner<T>;
	using TWatcher			= Watcher<T>;
public:
	Base() {}
	virtual ~Base() {}

	template <typename U = T*>
	U Get() {
		static_assert(!IsReferenceType_v<U>, "... cannot cast to reference type");	// Get<int&>와 같은 캐스팅을 방지
		static_assert(IsPointerType_v<U>, "... only cast to pointer type.");		// Get<int>	와 같은 방식을 방지

		if constexpr (Detail::IsStaticCastable<RemovePointer_t<U>, T>())
			return static_cast<U>(m_pPointer);
		else if constexpr (Detail::IsDynamicCastable<RemovePointer_t<U>, T>()) {
			U pCasted = dynamic_cast<U>(m_pPointer);
			ThrowIfDynamicCastingFailed<T*>(pCasted);
			return pCasted;
		} else {
			DebugAssertMsg(false, "... cannot conver to T_T");//static_assert(false, "... cannot conver to T_T");
		}
		return nullptr;
	}

	T& GetObj() {
		if (!Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return *m_pPointer;
	}

	void* GetRaw() const {
		return (void*)m_pPointer;
	}

	bool Exist() {
		if (m_pPointer == nullptr || m_pCounter == nullptr) {
			return false;
		}

		return m_pCounter->Alive;
	}

	int WatcherCount() const {
		if (m_pCounter == nullptr) {
			return false;
		}

		return m_pCounter->Counter;
	}


	T* operator->() {
		return Get();
	}

	T& operator*() {
		return GetObj();
	}
protected:
	enum class Cast
	{
		StaticCastable,
		DynamicCastable
	};

	void DeletePointer() {
		if (m_pPointer == nullptr) {
			return;
		}

		if (m_bMaked) {
			m_pCounter->DestroyObject();
		} else {
			JCORE_DELETE_SAFE(m_pPointer);
		}
		
		m_pCounter->Alive = false;
		SubtractWatcherCount();
	}

	void SubtractWatcherCount() {
		if (m_pCounter == nullptr) {
			return;
		}

		m_pCounter->Counter--;
		if (m_pCounter->Counter == 0) {
			JCORE_DELETE_SAFE(m_pCounter);
		}
	}

	void AddWatcherCount() const {
		if (m_pCounter == nullptr) {
			return;
		}

		m_pCounter->Counter++;
	}

	template <typename TI>	// 함수 정보 제공을 위해 일부러 달음
	static void ThrowIfDynamicCastingFailed(void* ptr) {
		if (ptr == nullptr) {
			throw NullPointerException("다이나믹 캐스팅에 실패하였습니다.");
		}
	}

	template <typename U>
	static void ThrowIfOwnerNotExist(Owner<U>& owner) {
		if (!owner.Exist()) {
			throw InvalidArgumentException("전달받은 오너의 정보가 비어있습니다.");
		}
	}

	template <typename U, Cast CastCheck>
	void OwnerMoveToOwner(Owner<U>& owner) {
		T* pTemp = nullptr;

		if constexpr (CastCheck == Cast::DynamicCastable) {
			pTemp = dynamic_cast<T*>(owner.m_pPointer);
			ThrowIfDynamicCastingFailed<T*>(pTemp);
		} else {
			pTemp = static_cast<T*>(owner.m_pPointer);
		}
		DeletePointer();

		m_pPointer = pTemp;
		m_pCounter = owner.m_pCounter;
		m_bMaked = owner.m_bMaked;

		owner.m_pPointer = nullptr;
		owner.m_pCounter = nullptr;
	}
	
	void WatcherMakeEmpty() {
		SubtractWatcherCount();

		m_pPointer = nullptr;
		m_pCounter = nullptr;
	}

	template <typename U, Cast CastCheck>
	void WatcherCopyToOwner(Owner<U>& owner) {
		T* pTemp = nullptr;
		if constexpr (CastCheck == Cast::DynamicCastable) {
			pTemp = dynamic_cast<T*>(owner.m_pPointer);
			ThrowIfDynamicCastingFailed<T*>(pTemp);
		} else {
			pTemp = static_cast<T*>(owner.m_pPointer);
		}
		SubtractWatcherCount();

		m_pPointer = pTemp;
		m_pCounter = owner.m_pCounter;

		AddWatcherCount();
	}


	template <typename U, Cast CastCheck>
	void WatcherCopyToWatcher(Watcher<U>& watcher) {
		T* pTemp = nullptr;
		if constexpr (CastCheck == Cast::DynamicCastable) {
			pTemp = dynamic_cast<T*>(watcher.m_pPointer);
			ThrowIfDynamicCastingFailed<T*>(pTemp);
		} else {
			pTemp = static_cast<T*>(watcher.m_pPointer);
		}
		SubtractWatcherCount();

		m_pPointer = pTemp;
		m_pCounter = watcher.m_pCounter;

		AddWatcherCount();
	}

	template <typename U, Cast CastCheck>
	void WatcherMoveToWatcher(Watcher<U>& watcher) {
		T* pTemp = nullptr;
		if constexpr (CastCheck == Cast::DynamicCastable) {
			pTemp = dynamic_cast<T*>(watcher.m_pPointer);
			ThrowIfDynamicCastingFailed<T*>(pTemp);
		} else {
			pTemp = static_cast<T*>(watcher.m_pPointer);
		}
		SubtractWatcherCount();

		m_pPointer = pTemp;
		m_pCounter = watcher.m_pCounter;

		watcher.m_pPointer = nullptr;
		watcher.m_pCounter = nullptr;
	}
protected:
	T* m_pPointer = nullptr;
	PtrCounter* m_pCounter = nullptr;
	bool m_bMaked = false;

	template <typename> friend class Base;
};




template <typename T>
class Owner : public Base<T>
{
	using TBase			= Base<T>;
	using TOwner		= Owner<T>;
	using TWatcher		= Watcher<T>;

	friend class TWatcher;


public:
	Owner(T* ptr, PtrCounter* counter) {
		this->m_pPointer = ptr;
		this->m_pCounter = counter;
		this->m_bMaked = true;
	}

	Owner(T* ptr) {
		this->m_pPointer = ptr;
		this->m_pCounter = dbg_new PtrCounter();
		this->m_bMaked = false;
	}

	template <typename U>
	Owner(const Owner<U>&) = delete;

	template <typename U>
	Owner(Owner<U>&& owner) {
		this->ThrowIfOwnerNotExist(owner);
		MoveToOwner(owner);
	}

	~Owner() override {
		this->DeletePointer();
	}

	bool Free() {
		if (this->m_pPointer == nullptr) {
			return false;
		}

		this->DeletePointer();
		return true;
	}

	void operator=(const TOwner&) = delete;

	template <typename U>
	TOwner& operator=(Owner<U>&& owner) {
		this->ThrowIfOwnerNotExist(owner);
		MoveToOwner(owner);
		return *this;
	}


	template <typename U>
	void MoveToOwner(Owner<U>& owner) {
		if constexpr (Detail::IsStaticCastable<T, U>()) {
			this->OwnerMoveToOwner<U, TBase::Cast::StaticCastable>(owner);
		} else if constexpr (Detail::IsDynamicCastable<T, U>()) {
			this->OwnerMoveToOwner<U, TBase::Cast::DynamicCastable>(owner);
		} else {
			DebugAssertMsg(false, "... cannot convert each other"); // static_assert(false, "cannot convert each other");
		}
	}
};



template <typename T>
class Watcher : public Base<T>
{
	using TBase			= Base<T>;
	using TOwner		= Owner<T>;
	using TWatcher		= Watcher<T>;

	friend class TOwner;
public:
	Watcher() {}
	Watcher(std::nullptr_t) {}

	template <typename U>
	Watcher(Owner<U>& owner) {
		CopyToOwner(owner);
	}

	template <typename U>
	Watcher(Watcher<U>& watcher) {
		CopyToWatcher(watcher);
	}

	template <typename U>
	Watcher(Watcher<U>&& watcher) {
		MoveToWatcher(watcher);
	}

	template <typename U>
	Watcher(Owner<U>&& monitor) = delete;

	~Watcher() override {
		this->SubtractWatcherCount();
	}

	template <typename U>
	TWatcher& operator=(Owner<U>& owner) {
		this->ThrowIfOwnerNotExist(owner);
		CopyToOwner(owner);
		return *this;
	}

	TWatcher& operator=(TOwner&& owner) = delete;

	TWatcher& operator=(std::nullptr_t) {
		this->WatcherMakeEmpty();
		return *this;
	}

	template <typename U>
	TWatcher& operator=(Watcher<U>& watcher) {
		CopyToWatcher(watcher);
		return *this;
	}


	template <typename U>
	TWatcher& operator=(Watcher<U>&& watcher) {
		MoveToWatcher(watcher);
		return *this;
	}


	template <typename U>
	void CopyToOwner(Owner<U>& owner) {
		if constexpr (Detail::IsStaticCastable<T, U>()) {
			this->WatcherCopyToOwner<U, TBase::Cast::StaticCastable>(owner);
		} else if constexpr (Detail::IsDynamicCastable<T, U>()) {
			this->WatcherCopyToOwner<U, TBase::Cast::DynamicCastable>(owner);
		} else {
			DebugAssertMsg(false, "... cannot convert each other"); //static_assert(false, "cannot convert each other");
		}
	}
	 
	template <typename U>
	void CopyToWatcher(Watcher<U>& watcher) {
		if (!watcher.Exist()) {
			this->WatcherMakeEmpty();
			return;
		}

		if constexpr (Detail::IsStaticCastable<T, U>()) {
			this->WatcherCopyToWatcher<U, TBase::Cast::StaticCastable>(watcher);
		} else if constexpr (Detail::IsDynamicCastable<T, U>()) {
			this->WatcherCopyToWatcher<U, TBase::Cast::DynamicCastable>(watcher);
		} else {
			DebugAssertMsg(false, "... cannot convert each other");
		}
	}

	template <typename U>
	void MoveToWatcher(Watcher<U>& watcher) {
		if (!watcher.Exist()) {
			this->WatcherMakeEmpty();
			return;
		}

		if constexpr (Detail::IsStaticCastable<T, U>()) {
			this->WatcherMoveToWatcher<U, TBase::Cast::StaticCastable>(watcher);
		} else if constexpr (Detail::IsDynamicCastable<T, U>()) {
			this->WatcherMoveToWatcher<U, TBase::Cast::DynamicCastable>(watcher);
		} else {
			DebugAssertMsg(false, "... cannot convert each other");
		}
	}
};



// 값 타입으로 반환
template <typename T, typename... Args>
constexpr decltype(auto) MakeOwner(Args&&... args) {
	OwnerObject<T>* object = dbg_new OwnerObject<T>(Forward<Args>(args)...);
	Owner<T> owner(object->Address(), object);
	return owner;
}


// 포인터 타입으로 반환
template <typename T, typename... Args>
constexpr decltype(auto) MakeOwnerPointer(Args&&... args) {
	OwnerObject<T>* object = dbg_new OwnerObject<T>(Forward<Args>(args)...);
	Owner<T>* owner = dbg_new Owner<T>(object->Address(), object);
	return owner;
}


// 글로벌 비교 오퍼레이터
template <typename T, typename U>
bool operator==(const Watcher<T>& lhs, const Owner<U>& rhs) {
	return lhs.GetRaw() == rhs.GetRaw();
}

template <typename T, typename U>
bool operator==(const Owner<T>& lhs, const Watcher<U>& rhs) {
	return lhs.GetRaw() == rhs.GetRaw();
}

template <typename T, typename U>
bool operator==(const Watcher<T>& lhs, const Watcher<U>& rhs) {
	return lhs.GetRaw() == rhs.GetRaw();
}

template <typename T, typename U>
bool operator==(std::nullptr_t, const Watcher<T>& rhs) {
	return nullptr == rhs.GetRaw();
}

template <typename T, typename U>
bool operator==(const Watcher<T>& lhs, std::nullptr_t) {
	return lhs.GetRaw() == nullptr;
}

template <typename T, typename U>
bool operator==(std::nullptr_t, const Owner<T>& rhs) {
	return nullptr == rhs.GetRaw();
}

template <typename T, typename U>
bool operator==(const Owner<T>& lhs, std::nullptr_t) {
	return lhs.GetRaw() == nullptr;
}


// 글로벌 비교 오퍼레이터
bool operator==(const VoidOwner& lhs, const VoidWatcher& rhs);
bool operator==(const VoidWatcher& lhs, const VoidOwner& rhs);
bool operator==(const VoidWatcher& lhs, const VoidWatcher& rhs);
bool operator==(const VoidWatcher& lhs, std::nullptr_t);
bool operator==(std::nullptr_t, const VoidWatcher& rhs);


NS_JC_END