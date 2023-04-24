/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:33:33 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/Struct.h>

#define CreateNew(Type) public: ActorListener* createNew() override { return dbg_new Type; }

class Actor;
class ActorPartAnimation;
class FrameTexture;
class ActorListener
{
public:
	ActorListener(ActorType_t type) : m_eActorType(type) {}
	virtual ~ActorListener() = default;

	virtual void injectActor(Actor* actor) = 0;
	virtual void onCreated() {}
	virtual void onUpdate(float dt) {}
	virtual void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture)	{}
	virtual void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {}
	virtual void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {}
	virtual void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {}

	virtual ActorListener* createNew() = 0;
	ActorType_t getActorType() { return m_eActorType; }
private:
	ActorType_t m_eActorType;
};


