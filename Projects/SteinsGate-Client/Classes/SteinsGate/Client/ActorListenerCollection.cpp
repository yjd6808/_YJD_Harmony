/*
 * 작성자: 윤정도
 * 생성일: 7/25/2023 8:34:11 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "ActorListenerCollection.h"

//////////////////////////////////////////////////////////////////////////////////////////
bool ActorListenerCollection::add(IActorListener* _pListener)
{
	const IActorListener::Type type = _pListener->getListenerType();

	if (m_hListeners.Exist(type))
	{
		return false;
	}

	m_hListeners.Insert(type, _pListener);
	m_vListeners.PushBack(_pListener);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ActorListenerCollection::has(IActorListener::Type _type)
{
	return m_hListeners.Exist(_type);
}

//////////////////////////////////////////////////////////////////////////////////////////
IActorListener* ActorListenerCollection::get(IActorListener::Type _type)
{
	IActorListener** pFoundListener = m_hListeners.Find(_type);
	if (pFoundListener == nullptr)
	{
		return nullptr;
	}

	return *pFoundListener;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::deleteAll()
{
	m_vListeners.ForEachDelete();
	m_vListeners.Clear();
	m_hListeners.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::onCreated()
{
	for (int i = 0; i < m_vListeners.Size(); ++i)
	{
		const auto pListener = m_vListeners[i];
		pListener->onCreated();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::onCleanUp()
{
	for (int i = 0; i < m_vListeners.Size(); ++i)
	{
		const auto pListener = m_vListeners[i];
		pListener->onCleanUp();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::onUpdate(float _dt)
{
	for (int i = 0; i < m_vListeners.Size(); ++i)
	{
		const auto pListener = m_vListeners[i];
		pListener->onUpdate(_dt);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	for (int i = 0; i < m_vListeners.Size(); ++i)
	{
		const auto pListener = m_vListeners[i];
		pListener->onAnimationBegin(_pAnimation, _pTexture);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	for (int i = 0; i < m_vListeners.Size(); ++i)
	{
		const auto pListener = m_vListeners[i];
		pListener->onAnimationEnd(_pAnimation, _pTexture);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	for (int i = 0; i < m_vListeners.Size(); ++i)
	{
		const auto pListener = m_vListeners[i];
		pListener->onFrameBegin(_pAnimation, _pTexture);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	for (int i = 0; i < m_vListeners.Size(); ++i)
	{
		const auto pListener = m_vListeners[i];
		pListener->onFrameEnd(_pAnimation, _pTexture);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::onHit(const HitInfo& _hitInfo)
{
	for (int i = 0; i < m_vListeners.Size(); ++i)
	{
		const auto pListener = m_vListeners[i];
		pListener->onHit(_hitInfo);
	}
}
