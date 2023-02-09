﻿/*
	작성자 : 윤정도
	해당 포인터가 살아있는지 여부를 알 수 있는 스마트 포인터, 쓰레드 세이프 하지 않음

	Owner : 포인터의 주인 => Owner 객체간에는 포인터 복사가 불가능하다.
	Watcher : 주인을 감시하는 놈 => 오너의 포인터 정보를 복사하여 접근하여 사용 가능
*/

#include <JCore/Core.h>
#include <JCore/Primitives/ObserverPtr.h>

NS_JC_BEGIN


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

void VoidBase::WatcherCopyToOwner(const VoidOwner& owner) {
	SubtractWatcherCount();

	m_pPointer = owner.m_pPointer;
	m_pCounter = owner.m_pCounter;

	AddWatcherCount();
}

void VoidBase::WatcherCopyToWatcher(const VoidWatcher& watcher) {
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




NS_JC_END