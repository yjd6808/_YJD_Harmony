/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 8:50:32 AM
 * =====================
 * 액터 노드 관계도
 *
 *                      SGActor
 *						   |
 *               ┌─────────┴────────────┐
 *		 SGDrawNode(두께 박스)      SGActorSprite
 *		                                 |
 *						             캔버스 노드
 *									     |
 *					             SGActorPartSprite
 *						                 |
 *				  		         SGDrawNode(히트박스)        
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/Struct.h>
#include <SteinsGate/Client/ActorSprite.h>
#include <SteinsGate/Client/HitRecorder.h>
#include <SteinsGate/Client/ActorListener.h>

class Actor;
class MapLayer;
class Actor : public SGNode
{
public:
	Actor(ActorType_t type, int code);
	~Actor() override;

	virtual void initActorSprite() = 0;
	virtual void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) = 0;
	virtual void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) = 0;
	virtual void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) = 0;
	virtual void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) = 0;
	virtual void initThicknessBox(const ThicknessBox& thicknessBox);
	virtual void initHitRecorder(int hitPossibleListSize = 16, int alreadyHitMapSize = 32);
	virtual void initListener(ActorListener* listener) = 0;	// 올바른 리스너인지 자식이 체크하도록함
	virtual bool isPhysicsActor() { return false; }

	


	virtual bool initVariables();
	void update(float dt) override;				// 자식에서도 오버라이딩시 이거 호출하도록

	ActorType_t getType()					const;
	const char* getTypeName()				const;
	SGActorRect getActorRect()				const;
	SGRect getThicknessBoxRect()			const;
	SGVec2 getPositionReal()				const;
	float  getPositionRealX()				const;
	float  getPositionRealY()				const;
	float  getPositionActorY()				const;
	SGVec2 getPositionRealCenter()			const;
	float  getPositionRealCenterX() 		const;
	float  getPositionRealCenterY()			const;
	SGVec2 getCanvasPositionReal()			const;
	SGSize getCanvasSize()					const;
	SGRect getHitBox()						const;
	ThicknessBox getThicknessBox()		const;
	ActorSprite* getActorSprite()			const;
	SpriteDirection_t getSpriteDirection()  const;
	int getRunningAnimationCode();
	ActorPartAnimation* getRunningAnimation();
	int getAllyFlag() { return m_iAllyFlag; }
	MapLayer* getMapLayer() { return m_pMapLayer; }
	HitRecorder* getHitRecorder() { return m_pHitRecorder; }
	int getActorId() { return m_iActorId; }
	int getCode() { return m_iCode; }
	ActorListener* getListener() { return m_pListener; }

	void setPositionReal(float x, float y);
	void setPositionReal(const SGVec2& v);
	void setPositionRealX(float x);
	void setPositionRealY(float y);
	void setPositionRealCenter(float x, float y);
	void setPositionRealCenter(const SGVec2& v);
	void setAllyFlag(int flag) { m_iAllyFlag = flag; }
	void setMapLayer(MapLayer* mapLayer);
	void setActorId(int id);

	void runAnimation(int animationCode);
	void runAnimation(int animationCode, int startFrameIndexInAnimation);
	void pauseAnimation(float delay);
	void runFrameEvent(FrameEventType_t frameEventType, int frameEventId);

	void setSpriteDirection(SpriteDirection_t direction);
	void setForwardDirection();
	void setBackwardDirection();

	bool isCollide(Actor* other, Out_ SpriteDirection_t& otherHitDirection, Out_ SGRect& hitRect);
	bool isCollide(const SGActorRect& otherRect, Out_ SpriteDirection_t& otherHitDirection, Out_ SGRect& hitRect);
	bool isCollide(const SGActorRect& otherRect);
	bool isOnTheGround();

	void cleanUpNext();
	

	void attach(Actor* actor);
	void detach(Actor* actor);
	Actor* getAttacher();
	bool hasAttacher();
	
public:
	// stdActor기준으로 절대 액터 렉트를 얻도록 해줌
	static SGActorRect convertAbsoluteActorRect(Actor* stdActor, const SGActorRect& relativeRect);
protected:
	MapLayer* m_pMapLayer;
	ActorType_t m_eActorType;
	ActorSprite* m_pActorSprite;
	HitRecorder* m_pHitRecorder;
	ActorListener* m_pListener;

	int m_iActorId;
	int m_iCode;
	int m_iAllyFlag;		// 쓸일 있을려나
	bool m_bCleanUp;

	SGDrawNode* m_pThicknessBox;

	SGVector<Actor*> m_vAttches;
	Actor* m_pAttacher;
};







