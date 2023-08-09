/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 4:14:21 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ComponentCollection.h"

ComponentCollection::ComponentCollection()
	: m_hComponentMap(0)
	, m_vUpdatables(0)
{}

ComponentCollection::ComponentCollection(int capacity)
	: m_hComponentMap(capacity)
	, m_vUpdatables(capacity)
{}

ComponentCollection::~ComponentCollection() {
	m_hComponentMap.ForEachValueDelete();
}

void ComponentCollection::clear() {
	m_hComponentMap.ForEachValueDelete();
	m_hComponentMap.Clear();
	m_vUpdatables.Clear();
}

bool ComponentCollection::add(IComponent* component) {
	const bool bAdded = m_hComponentMap.Insert(int(component->getType()), component);
	IUpdatable* pUpdatbleCompoenent = dynamic_cast<IUpdatable*>(component);

	if (bAdded && pUpdatbleCompoenent != nullptr) {
		m_vUpdatables.PushBack(pUpdatbleCompoenent);
	}

	return bAdded;
}

bool ComponentCollection::has(IComponent::Type type) const {
	return m_hComponentMap.Exist(int(type));
}

bool ComponentCollection::remove(IComponent::Type type) {
	IComponent** ppFoundComponent = m_hComponentMap.Find(int(type));

	if (ppFoundComponent == nullptr) {
		return false;
	}

	IUpdatable* pUpdatbleCompoenent = dynamic_cast<IUpdatable*>(*ppFoundComponent);

	if (pUpdatbleCompoenent) {
		const bool bRemoved = m_vUpdatables.Remove(pUpdatbleCompoenent);
		DebugAssert(bRemoved);
	}

	return true;
}

void ComponentCollection::onUpdate(float dt) {
	for (int i = 0; i < m_vUpdatables.Size(); ++i) {
		m_vUpdatables[i]->onUpdate(dt);
	}
}
