/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:33:33 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGStruct.h>

class SGActor;
class SGActorPartAnimation;
class SGFrameTexture;
class SGActorListener
{
public:
	SGActorListener(ActorType_t type) : m_eActorType(type) {}
	virtual ~SGActorListener() = default;

	virtual void injectActor(SGActor* actor) = 0;
	virtual void onCreated() {}
	virtual void onUpdate(float dt) {}
	virtual void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture)	{}
	virtual void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {}
	virtual void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {}
	virtual void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {}

	virtual SGActorListener* createNew() = 0;
	ActorType_t getActorType() { return m_eActorType; }
private:
	ActorType_t m_eActorType;
};


