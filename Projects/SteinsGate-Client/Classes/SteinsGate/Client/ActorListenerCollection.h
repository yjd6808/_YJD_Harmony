/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:33:33 AM
 * =====================
 * 여러 리스너들을 한번에 다루기위한 클래스
 * 각 액터당 리스너가 2개이상 필요한 경우가 없을 것 같긴한데.. 우선 둬보자.
 * 생각치 못하게 2개이상 사용하는 경우가 생길수 있으니.
 */

#pragma once

#include <SteinsGate/Client/IActorListener.h>

class ActorListenerCollection
{
public:
	ActorListenerCollection()
		: m_vListeners(1)
		, m_hListeners(1)
	{
	}

	bool add(IActorListener* _pListener);
	bool has(IActorListener::Type _type);
	IActorListener* get(IActorListener::Type _type);

	void deleteAll();

	void onCreated();
	void onCleanUp();
	void onUpdate(float _dt);
	void onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onHit(const HitInfo& _hitInfo);

protected:
	SGVector<IActorListener*> m_vListeners;
	SGHashMap<IActorListener::Type, IActorListener*> m_hListeners;
};
