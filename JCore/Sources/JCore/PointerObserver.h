/*
	작성자 : 윤정도
	유니크 포인터와 유사하고 해당 포인터가 살아있는지 여부를 알 수 있는 스마트 포인터
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Exception.h>

#include <atomic>

#define DeleteSafe(x)			\
do {							\
	if (x) {					\
		delete x;				\
	}							\
	x = nullptr;				\
} while (0);	


namespace JCore {


class VoidOwner;
class VoidWatcher;

struct PointerCounter {
	bool IsAlive() {
		return Alive.load();
	}

	bool Count() {
		return Counter.load();
	}

	std::atomic<bool> Alive = true;
	std::atomic<Int32U> Counter = 1;
};



/*=====================================================================================
									Void 포인터 베이스
 Void 왓쳐와 Void 오너의 기능을 포함하고 있는 객체
 =====================================================================================*/
class VoidBase {
public:
	VoidBase() {}
	virtual ~VoidBase() {}

	template <typename T>
	T* Get() {
		if (!Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return (T*)m_pPointer;
	}

	void* GetRaw() {
		if (!Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return m_pPointer;
	}


	bool Exist() {
		if (m_pPointer == nullptr || m_pCounter == nullptr) {
			return false;
		}

		return m_pCounter->Alive;
	}

	int WatcherCount() {
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

		DeleteSafe(m_pPointer);
		m_pCounter->Alive = false;
		m_pCounter->Counter--;
	}

	void SubtractWatcherCount() {
		if (m_pCounter == nullptr) {
			return;
		}

		m_pCounter->Counter--;
		if (m_pCounter->Counter == 0) {
			delete m_pCounter;
		}
	}

	void AddWatcherCount() {
		if (m_pCounter == nullptr) {
			return;
		}

		m_pCounter->Counter++;
	}

	void OwnerMoveToOwner(VoidOwner& owner);
	void WatcherCopyToOwner(VoidOwner& owner);
	void WatcherCopyToWatcher(VoidWatcher& watcher);
	void WatcherMoveToWatcher(VoidWatcher& watcher);
protected:
	void* m_pPointer = nullptr;
	PointerCounter* m_pCounter = nullptr;
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
	VoidOwner(void* ptr) {
		m_pPointer = ptr;
		m_pCounter = new PointerCounter();
	}

	VoidOwner(const VoidOwner&) = delete;
	VoidOwner(VoidOwner&& owner) noexcept {
		OwnerMoveToOwner(owner);
	}

	virtual ~VoidOwner() override {
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
};



/*=====================================================================================
									Void 포인터 감시자
 =====================================================================================*/

class VoidWatcher : public VoidBase
{
	friend class VoidOwner;
public:
	VoidWatcher() {}
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

	VoidWatcher& operator=(VoidWatcher&& watcher) {
		WatcherMoveToWatcher(watcher);
		return *this;
	}
};



void VoidBase::OwnerMoveToOwner(VoidOwner& owner) {
	DeletePointer();

	m_pPointer = owner.m_pPointer;
	m_pCounter = owner.m_pCounter;

	owner.m_pPointer = nullptr;
	owner.m_pCounter = nullptr;
}

void VoidBase::WatcherCopyToOwner(VoidOwner& owner) {
	SubtractWatcherCount();

	m_pPointer = owner.m_pPointer;
	m_pCounter = owner.m_pCounter;

	AddWatcherCount();
}

void VoidBase::WatcherCopyToWatcher(VoidWatcher& watcher) {
	SubtractWatcherCount();

	m_pPointer = watcher.m_pPointer;
	m_pCounter = watcher.m_pCounter;

	AddWatcherCount();
}

void VoidBase::WatcherMoveToWatcher(VoidWatcher& watcher) {
	SubtractWatcherCount();

	m_pPointer = watcher.m_pPointer;
	m_pCounter = watcher.m_pCounter;

	watcher.m_pPointer = nullptr;
	watcher.m_pCounter = nullptr;
}



/*=====================================================================================
								

								Void 오너/왓쳐의 템플릿 버전입니다.


 =====================================================================================*/





template <typename> class OwnerObject;
template <typename> class Owner;
template <typename> class Watcher;


template <typename T>
struct OwnerObject : PointerCounter
{
	template <typename... Args>
	explicit OwnerObject(Args&&... args) {
		::new (AddressOf(Object)) T{ Forward<Args>(args)... };
	}

	void DestroyObject() {
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
	using TOwnerObject		= typename OwnerObject<T>;
	using TOwner			= typename Owner<T>;
	using TWatcher			= typename Watcher<T>;
public:
	Base() {}
	virtual ~Base() {}

	T* Get() {
		if (!Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return m_pPointer;
	}

	T& GetObj() {
		if (!Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return *m_pPointer;
	}

	void* GetRaw() {
		if (!Exist()) {
			throw NullPointerException("포인터가 존재하지 않습니다.");
		}

		return (void*)m_pPointer;
	}


	bool Exist() {
		if (m_pPointer == nullptr || m_pCounter == nullptr) {
			return false;
		}

		return m_pCounter->Alive;
	}

	int WatcherCount() {
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

		if (m_bMaked) {
			((TOwnerObject*)m_pCounter)->DestroyObject();
		} else {
			DeleteSafe(m_pPointer);
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
			delete m_pCounter;
		}
	}

	void AddWatcherCount() {
		if (m_pCounter == nullptr) {
			return;
		}

		m_pCounter->Counter++;
	}

	void OwnerMoveToOwner(TOwner& owner) {
		DeletePointer();

		m_pPointer = owner.m_pPointer;
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

	void WatcherCopyToOwner(TOwner& owner) {
		SubtractWatcherCount();

		m_pPointer = owner.m_pPointer;
		m_pCounter = owner.m_pCounter;

		AddWatcherCount();
	}

	void WatcherCopyToWatcher(TWatcher& watcher) {
		SubtractWatcherCount();

		m_pPointer = watcher.m_pPointer;
		m_pCounter = watcher.m_pCounter;

		AddWatcherCount();
	}

	void WatcherMoveToWatcher(TWatcher& watcher) {
		SubtractWatcherCount();

		m_pPointer = watcher.m_pPointer;
		m_pCounter = watcher.m_pCounter;

		watcher.m_pPointer = nullptr;
		watcher.m_pCounter = nullptr;
	}
protected:
	T* m_pPointer = nullptr;
	PointerCounter* m_pCounter = nullptr;
	bool m_bMaked = false;
};




template <typename T>
class Owner : public Base<T>
{
	using TOwner		= typename Owner<T>;
	using TWatcher		= typename Watcher<T>;

	friend class TWatcher;


public:
	Owner(T* ptr, PointerCounter* counter) {
		this->m_pPointer = ptr;
		this->m_pCounter = counter;
		this->m_bMaked = true;
	}

	Owner(T* ptr) {
		this->m_pPointer = ptr;
		this->m_pCounter = new PointerCounter();
		this->m_bMaked = false;
	}

	
	Owner(const TOwner&) = delete;
	Owner(TOwner&& owner) noexcept {
		this->OwnerMoveToOwner(owner);
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
	TOwner& operator=(TOwner&& owner) {
		this->OwnerMoveToOwner(owner);
	}

	T* operator->() {
		return this->Get();
	}

	T& operator*() {
		return this->GetObj();
	}
};



template <typename T>
class Watcher : public Base<T>
{
	using TOwner		= typename Owner<T>;
	using TWatcher		= typename Watcher<T>;

	friend class TOwner;
public:
	Watcher() {}
	Watcher(std::nullptr_t) {}
	Watcher(TOwner& owner) {
		this->WatcherCopyToOwner(owner);
	}

	Watcher(TWatcher& watcher) {
		this->WatcherCopyToWatcher(watcher);
	}

	Watcher(TWatcher&& watcher) noexcept {
		this->WatcherMoveToWatcher(watcher);
	}

	Watcher(TOwner&& monitor) = delete;

	~Watcher() override {
		this->SubtractWatcherCount();
	}

	TWatcher& operator=(TOwner& owner) {
		this->WatcherCopyToOwner(owner);
		return *this;
	}

	TWatcher& operator=(TOwner&& owner) = delete;

	TWatcher& operator=(std::nullptr_t) {
		this->WatcherMakeEmpty();
		return *this;
	}

	TWatcher& operator=(TWatcher& watcher) {
		this->WatcherCopyToWatcher(watcher);
		return *this;
	}


	TWatcher& operator=(TWatcher&& watcher) {
		this->WatcherMoveToWatcher(watcher);
		return *this;
	}

	T* operator->() {
		return this->Get();
	}

	T& operator*() {
		return this->GetObj();
	}
};



// 값 타입으로 반환
template <typename T, typename... Args>
constexpr decltype(auto) MakeOwner(Args&&... args) {
	OwnerObject<T>* object = new OwnerObject<T>(Forward<Args>(args)...);
	Owner<T> owner(object->Address(), object);
	return owner;
}


// 포인터 타입으로 반환
template <typename T, typename... Args>
constexpr decltype(auto) MakeOwnerPointer(Args&&... args) {
	OwnerObject<T>* object = new OwnerObject<T>(Forward<Args>(args)...);
	Owner<T>* owner = new Owner<T>(object->Address(), object);
	return owner;
}









} // namespace JCore