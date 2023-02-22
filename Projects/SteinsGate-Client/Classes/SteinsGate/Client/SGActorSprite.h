/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:35 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/SGActorPartSprite.h>




class SGActor;
class SGActorSprite : public SGSprite
{
public:
	struct PartData
	{
		SGSprite* Canvas;
		SGActorPartSprite* Part;
		SGDrawNode* BoundingBox;
	};

	SGActorSprite(
		SGActor* actor, 
		const SGActorSpriteDataPtr& actorData
	);

	static SGActorSprite* create(
		SGActor* actor, 
		const SGActorSpriteDataPtr& actorData
	);

	bool init() override;
	void update(float dt) override;

	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture);

	void setSpriteDirection(SpriteDirection_t direction);
	void setForwardDirection();
	void setBackwardDirection();

	void runAnimation(int code);
	void runAnimation(int code, int startFrameIndexInAnimation);
	void pauseAnimation();
	void pauseAnimation(float delay);
	void resumeAnimation();

	SGActorPartSprite* getBodyPart();
	SGActor* getActor() { return m_pActor; }
	SGNode* getBodyCanvas();
	SGSize	getBodyCanvasSize();
	SGSize	getBodyPartSize();
	SGVec2	getBodyPartPosition();
	PartData createPart(const ActorPartSpriteData& partSpriteData, int partIndex, int frameCount);

	SGVector<PartData>& getParts() { return m_vParts; }
	SpriteDirection_t getSpriteDirection();
	int getRunningAnimationCode();
	SGActorPartAnimation* getRunningAnimation();
	void updateSpriteData(const SGActorSpriteDataPtr& spriteData);
private:


	SGActor* m_pActor;
	SGActorSpriteDataPtr m_spActorData;
	SGVector<PartData> m_vParts;
	//SGVector<SGSprite*> m_vPartsCanvas;
	//SGVector<SGActorPartSprite*> m_vParts;
	//SGVector<SGDrawNode*> m_vPartsBoundingBox;
	SpriteDirection_t m_eDirection;
};


