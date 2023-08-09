/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:33:33 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

#define SG_ACTOR_LISTENER_CREATE(Type) public: ActorListener* createNew() override { return dbg_new Type; }

class ActorPartAnimation;
class FrameTexture;
class JCORE_NOVTABLE ActorListener
{
public:
	enum Type
	{
		eCharacter,
		eProjectile
	};

	ActorListener() : m_pActor(nullptr) {}
	virtual ~ActorListener() = default;

	virtual void injectActor(Actor* actor) { m_pActor = actor; }
	virtual void onCreated() {}		// 
	virtual void onCleanUp();		// 엑터 박스에서 제거되어 더이상 사용하지 않는 경우
	virtual void onUpdate(float dt) {}
	virtual void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture)	{}
	virtual void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {}
	virtual void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {}
	virtual void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {}
	virtual Type getListenerType() const = 0;

	virtual ActorListener* createNew() = 0;
	Actor* getActor() const { return m_pActor; }
	ActorType_t getActorType() const;
	
protected:
	Actor* m_pActor;
};


