/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:33:33 AM
 * =====================
 * 여러 리스너들을 한번에 다루기위한 클래스
 * 각 액터당 리스너가 2개이상 필요한 경우가 없을 것 같긴한데.. 우선 둬보자.
 * 생각치 못하게 2개이상 사용하는 경우가 생길수 있으니.
 */

#pragma once

#include <sgcl/IActorListener.h>

struct HitInfo;

class ActorListenerCollection
{
public:
	ActorListenerCollection()
	: listeners_(1)
	, listenerMap_(1)
	{
	}

	bool Add(IActorListener* _pListener);
	bool Has(IActorListener::Type _type);
	IActorListener* Get(IActorListener::Type _type);

	void DeleteAll();

	void OnCreated();
	void OnCleanUp();
	void OnUpdate(float _dt);
	void OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnHit(const HitInfo& _hitInfo);

protected:
	jc::Vector<IActorListener*> listeners_;
	jc::HashMap<IActorListener::Type, IActorListener*> listenerMap_;
};
