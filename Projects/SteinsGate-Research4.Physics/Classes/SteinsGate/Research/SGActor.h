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

class SGActor : public SGNode
{
public:
	SGActor(ActorType_t type, int code);

	virtual void initActorSprite() = 0;
	virtual void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) = 0;
	virtual void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) = 0;
	virtual void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) = 0;
	virtual void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) = 0;
	virtual void initThicknessBox(const SGThicknessBox& thicknessBox);

	bool init() override;
	
	void update(float dt) override;				// 자식에서도 오버라이딩시 이거 호출하도록

	SGRect getThicknessBoxRect()		const;
	SGVec2 getPositionReal()			const;// 두께박스 좌하단의 위치
	SGVec2 getPositionRealCenter()		const;// 두께박스 중앙의 위치
	SGVec2 getCanvasPositionReal()		const;// 엑터 스프라이트의 절대 캔버스 위치
	SGSize getCanvasSize()				const;// 엑터 스프라이트의 바디 캔버스 사이즈
	SGRect getHitbox()					const;
	SGThicknessBox getThicknessBox()	const;
	SGActorSprite* getActorSprite()		const;
	SpriteDirection_t getSpriteDirection() const;

	void setPositionReal(float x, float y);
	void setPositionRealCenter(float x, float y);
	void runAnimation(int animationCode);

	void setSpriteDirection(SpriteDirection_t direction);
	void setForwardDirection();
	void setBackwardDirection();
	
protected:
	ActorType_t m_eActorType;
	SGActorSprite* m_pActorSprite;
	int m_iCode;

	SGDrawNode* m_pThicknessBox;
};



