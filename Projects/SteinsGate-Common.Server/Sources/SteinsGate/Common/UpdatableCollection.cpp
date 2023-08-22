/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:18:15 AM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "UpdatableCollection.h"

void UpdatableCollection::Update(const JCore::TimeSpan& elapsed) {
	const int iSize = m_vUpdatable.Size();
	for (int i = 0; i < iSize; ++i) {
		m_vUpdatable[i]->OnUpdate(elapsed);
	}
}

bool UpdatableCollection::Add(int id, IUpdatable* updatable) {

	if (!m_hUpdatableMap.Insert(id, updatable)) {
		return false;
	}
	m_vUpdatable.PushBack(updatable);
	return true;
}

bool UpdatableCollection::Has(int id) {
	return m_hUpdatableMap.Exist(id);
}

bool UpdatableCollection::Has(IUpdatable* updatable) {
	for (int i = 0; i < m_vUpdatable.Size(); ++i) {
		if (m_vUpdatable[i] == updatable) {
			return true;
		}
	}

	return false;
}

bool UpdatableCollection::Remove(int id) {

	IUpdatable** pUpdatable = m_hUpdatableMap.Find(id);
	if (pUpdatable == nullptr) {
		return false;
	}

	if (m_vUpdatable.Remove(*pUpdatable) && m_hUpdatableMap.Remove(id)) {
		return true;
	}

	DebugAssert(false);
	return false;
}

bool UpdatableCollection::Remove(IUpdatable* updatable) {

	bool bFound = false;
	int iFoundId = InvalidValue_v;

	m_hUpdatableMap.ForEach([&](JCore::Pair<int, IUpdatable*> pair) {
		if (bFound) return;
		if (updatable == pair.Value) {
			bFound = true;
			iFoundId = pair.Key;
		}
	});

	if (bFound)
		return m_hUpdatableMap.Remove(iFoundId) && m_vUpdatable.Remove(updatable);
	
	return false;
}

void UpdatableCollection::Clear() {
	m_hUpdatableMap.Clear();
	m_vUpdatable.Clear();
}
