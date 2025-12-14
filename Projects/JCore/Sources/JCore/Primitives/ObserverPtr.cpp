/*
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

//////////////////////////////////////////////////////////////////////////////////////////
void JCore::VoidBase::OwnerMoveToOwner(CVoidOwner& _owner)
{
	DeletePointer();

	m_pPointer = _owner.m_pPointer;
	m_pCounter = _owner.m_pCounter;

	_owner.m_pPointer = nullptr;
	_owner.m_pCounter = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void VoidBase::WatcherCopyToOwner(const CVoidOwner& _owner)
{
	SubtractWatcherCount();

	m_pPointer = _owner.m_pPointer;
	m_pCounter = _owner.m_pCounter;

	AddWatcherCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
void VoidBase::WatcherCopyToWatcher(const CVoidWatcher& _watcher)
{
	SubtractWatcherCount();

	m_pPointer = _watcher.m_pPointer;
	m_pCounter = _watcher.m_pCounter;

	AddWatcherCount();
}

//////////////////////////////////////////////////////////////////////////////////////////
void VoidBase::WatcherMoveToWatcher(CVoidWatcher& _watcher)
{
	SubtractWatcherCount();

	m_pPointer = _watcher.m_pPointer;
	m_pCounter = _watcher.m_pCounter;

	_watcher.m_pPointer = nullptr;
	_watcher.m_pCounter = nullptr;
}

// 글로벌 비교 오퍼레이터
//////////////////////////////////////////////////////////////////////////////////////////
bool operator==(const CVoidOwner& _lhs, const CVoidWatcher& _rhs)
{
	return _lhs.GetRaw() == _rhs.GetRaw();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool operator==(const CVoidWatcher& _lhs, const CVoidOwner& _rhs)
{
	return _lhs.GetRaw() == _rhs.GetRaw();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool operator==(const CVoidWatcher& _lhs, const CVoidWatcher& _rhs)
{
	return _lhs.GetRaw() == _rhs.GetRaw();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool operator==(const CVoidWatcher& _lhs, std::nullptr_t)
{
	return _lhs.GetRaw() == nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool operator==(std::nullptr_t, const CVoidWatcher& _rhs)
{
	return nullptr == _rhs.GetRaw();
}




NS_JC_END