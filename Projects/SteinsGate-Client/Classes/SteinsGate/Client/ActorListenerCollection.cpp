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
bool ActorListenerCollection::Add(IActorListener* _pListener)
{
	const IActorListener::Type type = _pListener->GetListenerType();

	if (listenerMap_.Exist(type))
	{
		return false;
	}

	listenerMap_.Insert(type, _pListener);
	listeners_.PushBack(_pListener);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ActorListenerCollection::Has(IActorListener::Type _type)
{
	return listenerMap_.Exist(_type);
}

//////////////////////////////////////////////////////////////////////////////////////////
IActorListener* ActorListenerCollection::Get(IActorListener::Type _type)
{
	IActorListener** pFoundListener = listenerMap_.Find(_type);
	if (pFoundListener == nullptr)
	{
		return nullptr;
	}

	return *pFoundListener;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::DeleteAll()
{
	listeners_.ForEachDelete();
	listeners_.Clear();
	listenerMap_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::OnCreated()
{
	for (int i = 0; i < listeners_.Size(); ++i)
	{
		const auto pListener = listeners_[i];
		pListener->OnCreated();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::OnCleanUp()
{
	for (int i = 0; i < listeners_.Size(); ++i)
	{
		const auto pListener = listeners_[i];
		pListener->OnCleanUp();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::OnUpdate(float _dt)
{
	for (int i = 0; i < listeners_.Size(); ++i)
	{
		const auto pListener = listeners_[i];
		pListener->OnUpdate(_dt);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	for (int i = 0; i < listeners_.Size(); ++i)
	{
		const auto pListener = listeners_[i];
		pListener->OnAnimationBegin(_pAnimation, _pTexture);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	for (int i = 0; i < listeners_.Size(); ++i)
	{
		const auto pListener = listeners_[i];
		pListener->OnAnimationEnd(_pAnimation, _pTexture);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	for (int i = 0; i < listeners_.Size(); ++i)
	{
		const auto pListener = listeners_[i];
		pListener->OnFrameBegin(_pAnimation, _pTexture);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	for (int i = 0; i < listeners_.Size(); ++i)
	{
		const auto pListener = listeners_[i];
		pListener->OnFrameEnd(_pAnimation, _pTexture);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorListenerCollection::OnHit(const HitInfo& _hitInfo)
{
	for (int i = 0; i < listeners_.Size(); ++i)
	{
		const auto pListener = listeners_[i];
		pListener->OnHit(_hitInfo);
	}
}
