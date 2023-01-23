/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:35 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGActorPartSprite.h>

class SGActor;
class SGActorSprite : public SGSprite
{
public:
	SGActorSprite(
		SGActor* actor, 
		const SGActorSpriteDataPtr& actorData, 
		bool attachPartsBoundingBoxToCanvas 
	);

	static SGActorSprite* create(
		SGActor* actor, 
		const SGActorSpriteDataPtr& actorData, 
		bool attachPartsToCanvas = false
	);

	bool init() override;
	void update(float dt) override;
	void runAnimation(int code);

	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void setSpriteDirection(SpriteDirection_t direction);
	void setForwardDirection();
	void setBackwardDirection();

	SGActorPartSprite* getBodyPart();
	SGNode* getBodyCanvas();
	SGSize	getBodyCanvasSize();
	SGSize	getBodyPartSize();
	SGVec2	getBodyPartPosition();

	SGVector<SGActorPartSprite*>& getParts() { return m_vParts; }
	SpriteDirection_t getSpriteDirection();
private:
	bool m_bPartsBoundingBoxAttachedToCanvas;		// 파츠 바운딩박스노드들이 액터 스프라이트 파츠가 아닌 캔버스에 붙였는지

	SGActor* m_pActor;
	SGActorSpriteDataPtr m_spActorData;
	SGVector<SGSprite*> m_vPartsCanvas;
	SGVector<SGActorPartSprite*> m_vParts;
	SGVector<SGDrawNode*> m_vPartsBoundingBox;
	SpriteDirection_t m_eDirection;
};


