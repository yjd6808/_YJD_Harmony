/*
	작성자 : 윤정도
	
*/

#include <JCore/Core.h>
#include <JCore/PointerObserver.h>

namespace JCore {


/*=====================================================================================
									Void 포인터 베이스
 Void 왓쳐와 Void 오너의 기능을 포함하고 있는 객체
 =====================================================================================*/

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

// 글로벌 비교 오퍼레이터
bool operator==(const VoidOwner& lhs, const VoidWatcher& rhs) {
	return lhs.GetRaw() == rhs.GetRaw();
}

bool operator==(const VoidWatcher& lhs, const VoidOwner& rhs) {
	return lhs.GetRaw() == rhs.GetRaw();
}

bool operator==(const VoidWatcher& lhs, const VoidWatcher& rhs) {
	return lhs.GetRaw() == rhs.GetRaw();
}

bool operator==(const VoidWatcher& lhs, std::nullptr_t) {
	return lhs.GetRaw() == nullptr;
}

bool operator==(std::nullptr_t, const VoidWatcher& rhs) {
	return nullptr == rhs.GetRaw();
}




} // namespace JCore