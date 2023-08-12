/*
 * 작성자: 윤정도
 * 생성일: 7/25/2023 8:34:11 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "ActorListenerCollection.h"

bool ActorListenerCollection::add(IActorListener* listener) {
	const IActorListener::Type eType = listener->getListenerType();

	if (m_hListeners.Exist(eType)) {
		return false;
	}

	m_hListeners.Insert(listener->getListenerType(), listener);
	m_vListeners.PushBack(listener);
	return true;
}

bool ActorListenerCollection::has(IActorListener::Type type) {
	return m_hListeners.Exist(type);
}

IActorListener* ActorListenerCollection::get(IActorListener::Type type) {
	IActorListener** ppFindListner = m_hListeners.Find(type);
	if (ppFindListner == nullptr) {
		return nullptr;
	}
	return *ppFindListner;
}

void ActorListenerCollection::deleteAll() {
	m_vListeners.ForEachDelete();
	m_vListeners.Clear();
	m_hListeners.Clear();
}

void ActorListenerCollection::onCreated() {
	for (int i = 0; i < m_vListeners.Size(); ++i) {
		const auto pListener = m_vListeners[i];
		pListener->onCreated();
	}
}

void ActorListenerCollection::onCleanUp() {
	for (int i = 0; i < m_vListeners.Size(); ++i) {
		const auto pListener = m_vListeners[i];
		pListener->onCleanUp();
	}
}

void ActorListenerCollection::onUpdate(float dt) {
	for (int i = 0; i < m_vListeners.Size(); ++i) {
		const auto pListener = m_vListeners[i];
		pListener->onUpdate(dt);
	}
}

void ActorListenerCollection::onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	for (int i = 0; i < m_vListeners.Size(); ++i) {
		const auto pListener = m_vListeners[i];
		pListener->onAnimationBegin(animation, texture);
	}
}

void ActorListenerCollection::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	for (int i = 0; i < m_vListeners.Size(); ++i) {
		const auto pListener = m_vListeners[i];
		pListener->onAnimationEnd(animation, texture);
	}
}

void ActorListenerCollection::onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	for (int i = 0; i < m_vListeners.Size(); ++i) {
		const auto pListener = m_vListeners[i];
		pListener->onFrameBegin(animation, texture);
	}
}

void ActorListenerCollection::onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	for (int i = 0; i < m_vListeners.Size(); ++i) {
		const auto pListener = m_vListeners[i];
		pListener->onFrameEnd(animation, texture);
	}
}

void ActorListenerCollection::onHit(const HitInfo& hitInfo) {
	for (int i = 0; i < m_vListeners.Size(); ++i) {
		const auto pListener = m_vListeners[i];
		pListener->onHit(hitInfo);
	}
}
