/*
	�ۼ��� : ������
	
*/

#include <JCore/Core.h>
#include <JCore/PointerObserver.h>

namespace JCore {


/*=====================================================================================
									Void ������ ���̽�
 Void ���Ŀ� Void ������ ����� �����ϰ� �ִ� ��ü
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

// �۷ι� �� ���۷�����
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