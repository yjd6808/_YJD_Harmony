/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:33:33 AM
 * =====================
 * 여러 리스너들을 한번에 다루기위한 클래스
 */


#pragma once

#include <SteinsGate/Client/ActorListener.h>

class ActorListenerCollection
{
public:
	ActorListenerCollection()
		: m_vListeners(1)
		, m_hListeners(1)
	{}

	bool add(ActorListener* listener);
	bool has(ActorListener::Type type);
	ActorListener* get(ActorListener::Type type);
	
	void deleteAll();

	void onCreated();
	void onCleanUp();
	void onUpdate(float dt);
	void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture);
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture);
	void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture);
	void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture);
	
protected:
	SGVector<ActorListener*> m_vListeners;
	SGHashMap<ActorListener::Type, ActorListener*> m_hListeners;
};