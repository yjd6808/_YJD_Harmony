/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 5:04:25 PM
 * =====================
 *
 */



#include "Server.h"
#include "ServerCoreHeader.h"
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
	m_hComponentMap.ForEachValueRelease();
}

void ComponentCollection::Clear() {
	m_hComponentMap.ForEachValueRelease();
	m_hComponentMap.Clear();
	m_vUpdatables.Clear();
}

bool ComponentCollection::Add(IComponent* component) {
	const bool bAdded = m_hComponentMap.Insert(component->GetType(), component);
	IUpdatable* pUpdatbleCompoenent = dynamic_cast<IUpdatable*>(component);
	ISessionEventHandler* pSessionEventHandlerComponent = dynamic_cast<ISessionEventHandler*>(component);

	if (bAdded && pUpdatbleCompoenent != nullptr) {
		m_vUpdatables.PushBack(pUpdatbleCompoenent);
	}

	if (bAdded && pSessionEventHandlerComponent != nullptr) {
		m_vSessionEventHandlers.PushBack(pSessionEventHandlerComponent);
	}

	return bAdded;
}

bool ComponentCollection::Has(int type) const {
	return m_hComponentMap.Exist(type);
}

bool ComponentCollection::Remove(int type) {
	IComponent** ppFoundComponent = m_hComponentMap.Find(type);

	if (ppFoundComponent == nullptr) {
		return false;
	}

	IUpdatable* pUpdatbleCompoenent = dynamic_cast<IUpdatable*>(*ppFoundComponent);
	ISessionEventHandler* pSessionEventHandlerComponent = dynamic_cast<ISessionEventHandler*>(*ppFoundComponent);

	if (pUpdatbleCompoenent) {
		const bool bRemoved = m_vUpdatables.Remove(pUpdatbleCompoenent);
		DebugAssert(bRemoved);
	}

	if (pSessionEventHandlerComponent) {
		const bool bRemoved = m_vSessionEventHandlers.Remove(pSessionEventHandlerComponent);
		DebugAssert(bRemoved);
	}

	return true;
}

void ComponentCollection::Initialize() {
	m_hComponentMap.ForEachValue([](IComponent* component) { component->Initialize(); });
}

void ComponentCollection::OnUpdate(const JCore::TimeSpan& elapsed) {
	for (int i = 0; i < m_vUpdatables.Size(); ++i) {
		m_vUpdatables[i]->OnUpdate(elapsed);
	}
}

void ComponentCollection::OnConnected() {
	for (int i = 0; i < m_vSessionEventHandlers.Size(); ++i) {
		m_vSessionEventHandlers[i]->OnConnected();
	}
}

void ComponentCollection::OnDisconnected() {
	for (int i = 0; i < m_vSessionEventHandlers.Size(); ++i) {
		m_vSessionEventHandlers[i]->OnDisconnected();
	}
}
