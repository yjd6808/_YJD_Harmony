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

class SGMapLayer;
class SGActor : public SGNode
{
public:
	SGActor(ActorType_t type, int code, SGMapLayer* mapLayer);
	~SGActor() override = default;

	virtual void initActorSprite() = 0;
	virtual void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) = 0;
	virtual void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) = 0;
	virtual void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) = 0;
	virtual void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) = 0;
	// virtual void onHit(SGActor* attacker, )
	virtual void initThicknessBox(const SGThicknessBox& thicknessBox);
	virtual bool isPhysicsActor() { return false; }

	bool init() override;
	void update(float dt) override;				// 자식에서도 오버라이딩시 이거 호출하도록

	ActorType_t getType()					const;
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
	int getAllyFlag() { return m_iAllyFlag; }
	SGMapLayer* getMapLayer() { return m_pBelongedMap; }

	void setPositionReal(float x, float y);
	void setPositionReal(const SGVec2& v);
	void setPositionRealX(float x);
	void setPositionRealY(float y);
	void setPositionRealCenter(float x, float y);
	void setPositionRealCenter(const SGVec2& v);
	void setAllyFlag(int flag) { m_iAllyFlag = flag; }

	void runAnimation(int animationCode);
	void runAnimation(int animationCode, int startFrameIndex);
	void runFrameEvent(FrameEventType_t frameEventType, int frameEventId);

	void setSpriteDirection(SpriteDirection_t direction);
	void setForwardDirection();
	void setBackwardDirection();

	
	
	bool isCollide(SGActor* other, Out_ SpriteDirection_t& otherHitDirection, Out_ SGRect& hitRect);
protected:
	SGMapLayer* m_pBelongedMap;
	ActorType_t m_eActorType;
	SGActorSprite* m_pActorSprite;
	int m_iCode;
	int m_iAllyFlag;

	SGDrawNode* m_pThicknessBox;
};



