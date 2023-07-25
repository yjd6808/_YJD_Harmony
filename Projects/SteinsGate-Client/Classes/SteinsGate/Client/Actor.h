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
#include <SteinsGate/Client/ActorListenerCollection.h>

class Actor;
class MapLayer;
class Actor : public SGNode
{
public:
	enum CleanUpFlag
	{
		CF_None					= 0,
		CF_ReleaseActorSprite	= 1
	};

	Actor(ActorType_t type, int code);
	~Actor() override;

	virtual void initActorSprite() = 0;
	virtual void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture);
	virtual void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture);
	virtual void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture);
	virtual void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture);
	virtual void initThicknessBox(const ThicknessBox& thicknessBox);
	virtual void initHitRecorder(int hitPossibleListSize = 16, int alreadyHitMapSize = 32, Actor* owner = nullptr);
	virtual bool isPhysicsActor() { return false; }
	virtual bool initVariables();

	bool addListener(ActorListener* listener);
	bool hasListener(ActorListener::Type type);
	ActorListener* getListener(ActorListener::Type type);
	void update(float dt) override;				// 자식에서도 오버라이딩시 이거 호출하도록

	ActorType_t getType()					const;
	const char* getTypeName()				const;
	ActorRect getActorRect()				const;
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
	ThicknessBox getThicknessBox()			const;
	SGDrawNode* getThicknessBoxNode()		const;
	ActorSprite* getActorSprite()			const;
	SpriteDirection_t getSpriteDirection()  const;
	int getRunningAnimationCode();
	ActorPartAnimation* getRunningAnimation();
	int getAllyFlag() { return m_iAllyFlag; }
	MapLayer* getMapLayer() { return m_pMapLayer; }
	HitRecorder* getHitRecorder() { return m_pHitRecorder; }
	int getActorId() { return m_iActorId; }
	virtual int getCode() = 0;
	ActorListenerCollection& getListenerCollection() { return m_vListeners; }

	void setPositionReal(float x, float y);
	void setPositionReal(const SGVec2& v);
	void setPositionRealX(float x);
	void setPositionRealY(float y);
	void setPositionRealCenter(float x, float y);
	void setPositionRealCenter(const SGVec2& v);
	void setAllyFlag(int flag) { m_iAllyFlag = flag; }
	void setMapLayer(MapLayer* mapLayer);
	void setActorId(int id);
	void setCleanUpFlag(int flag) { m_iCleanUpFlag = flag; }
	bool hasCleanUpFlag(int cleanUpFlag);
	void addCleanUpFlag(int cleanUpFlag);
	void clearCleanUpFlag();

	void runAnimation(int animationCode);
	void runAnimation(int animationCode, int startFrameIndexInAnimation);
	void pauseAnimation(float delay);

	void runFrameEventSpawn(FrameEventSpawnType_t frameEventSpawnType, int code);
	void runFrameEvent(FrameEvent* frameEvent);

	void setSpriteDirection(SpriteDirection_t direction);
	void setForwardDirection();
	void setBackwardDirection();

	bool isCollide(Actor* other, JCORE_OUT SpriteDirection_t& otherHitDirection, JCORE_OUT SGRect& hitRect);
	bool isCollide(const ActorRect& otherRect, JCORE_OUT SpriteDirection_t& otherHitDirection, JCORE_OUT SGRect& hitRect);
	bool isCollide(const ActorRect& otherRect);
	bool isOnTheGround();

	void releaseActorSprite();	// 액터의 텍스쳐를 수동으로 제거하기 위해

	void cleanUpAtNextFrame();
	void cleanUp();

	void attach(Actor* actor);	// 이 액터에 전달받은 액터를 붙임
	void detach(Actor* actor);	// 이 액터에게서 전달받은 액터를 떼어냄
	Actor* getAttacher();
	bool hasAttacher();
	

public:
	// stdActor기준으로 절대 액터 렉트를 얻도록 해줌
	static ActorRect convertAbsoluteActorRect(Actor* stdActor, const ActorRect& relativeRect);
protected:
	MapLayer* m_pMapLayer;
	ActorType_t m_eActorType;
	ActorSprite* m_pActorSprite;
	HitRecorder* m_pHitRecorder;
	ActorListenerCollection m_vListeners;

	int m_iActorId;					// 액터 박스에게서 부여된 고유 ID
	int m_iAllyFlag;				// 값이 같으면 동맹, 다르면 적
	int m_iCleanUpFlag;				// 클린업 될 때 수행할 작업 플래그
	bool m_bCleanUp;				// 액터가 될 예정 혹은 제거되었는지 여부

	SGDrawNode* m_pThicknessBox;

	
	SGVector<Actor*> m_vAttches;			// 내게 붙어 있는 녀석들
	Actor* m_pAttacher;						// 내가 누구에게 붙어있는가?
};







