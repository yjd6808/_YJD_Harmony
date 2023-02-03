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

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGStruct.h>
#include <SteinsGate/Research/SGActorSprite.h>
#include <SteinsGate/Research/SGHitRecorder.h>
#include <SteinsGate/Research/SGActorListener.h>

class SGMapLayer;
class SGActor : public SGNode
{
public:
	SGActor(ActorType_t type, int code);
	~SGActor() override;

	virtual void initActorSprite() = 0;
	virtual void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) = 0;
	virtual void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) = 0;
	virtual void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) = 0;
	virtual void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) = 0;
	// virtual void onHit(SGActor* attacker, )
	virtual void initThicknessBox(const SGThicknessBox& thicknessBox);
	virtual void initHitRecorder(int hitPossibleListSize = 16, int alreadyHitMapSize = 32);
	virtual void initListener(SGActorListener* listener) = 0;	// 올바른 리스너인지 자식이 체크하도록함
	virtual bool isPhysicsActor() { return false; }

	bool init() override;
	void update(float dt) override;				// 자식에서도 오버라이딩시 이거 호출하도록

	ActorType_t getType()					const;
	SGActorRect getActorRect()				const;
	SGRect getThicknessBoxRect()			const;
	SGVec2 getPositionReal()				const;
	float  getPositionRealX()				const;
	float  getPositionRealY()				const;
	float  getPositionActorY()				const;
	SGVec2 getPositionRealCenter()			const;
	float  getPositionRealCenterX() 		const;
	SGVec2 getCanvasPositionReal()			const;
	SGSize getCanvasSize()					const;
	SGRect getHitBox()						const;
	SGThicknessBox getThicknessBox()		const;
	SGActorSprite* getActorSprite()			const;
	SpriteDirection_t getSpriteDirection()  const;
	int getRunningAnimationCode();
	SGActorPartAnimation* getRunningAnimation();
	int getAllyFlag() { return m_iAllyFlag; }
	SGMapLayer* getMapLayer() { return m_pMapLayer; }
	SGHitRecorder* getHitRecorder() { return m_pHitRecorder; }
	int getActorId() { return m_iActorId; }
	SGActorListener* getListener() { return m_pListener; }

	void setPositionReal(float x, float y);
	void setPositionReal(const SGVec2& v);
	void setPositionRealX(float x);
	void setPositionRealY(float y);
	void setPositionRealCenter(float x, float y);
	void setPositionRealCenter(const SGVec2& v);
	void setAllyFlag(int flag) { m_iAllyFlag = flag; }
	void setMapLayer(SGMapLayer* mapLayer);
	void setActorId(int id);

	void runAnimation(int animationCode);
	void runAnimation(int animationCode, int startFrameIndexInAnimation);
	void pauseAnimation(float delay);
	void runFrameEvent(FrameEventType_t frameEventType, int frameEventId);

	void setSpriteDirection(SpriteDirection_t direction);
	void setForwardDirection();
	void setBackwardDirection();

	bool isCollide(SGActor* other, Out_ SpriteDirection_t& otherHitDirection, Out_ SGRect& hitRect);
	bool isCollide(const SGActorRect& otherRect, Out_ SpriteDirection_t& otherHitDirection, Out_ SGRect& hitRect);
	bool isCollide(const SGActorRect& otherRect);
	bool isOnTheGround();

	void registerCleanUp();
public:
	// stdActor기준으로 절대 액터 렉트를 얻도록 해줌
	static SGActorRect convertAbsoluteActorRect(SGActor* stdActor, const SGActorRect& relativeRect);
protected:
	SGMapLayer* m_pMapLayer;
	ActorType_t m_eActorType;
	SGActorSprite* m_pActorSprite;
	SGHitRecorder* m_pHitRecorder;
	SGActorListener* m_pListener;

	int m_iActorId;
	int m_iCode;
	int m_iAllyFlag;		// 쓸일 있을려나

	SGDrawNode* m_pThicknessBox;
};





