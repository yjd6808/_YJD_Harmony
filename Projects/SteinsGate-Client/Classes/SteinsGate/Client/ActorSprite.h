/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:35 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/ActorPartSprite.h>




class Actor;
class ActorSprite : public SGSprite
{
public:
	struct PartData
	{
		SGSprite* Canvas;
		ActorPartSprite* Part;
		SGDrawNode* BoundingBox;

		bool operator==(const PartData& other);
	};

	ActorSprite(
		Actor* actor, 
		const SGActorSpriteDataPtr& actorData
	);

	static ActorSprite* create(
		Actor* actor, 
		const SGActorSpriteDataPtr& actorData
	);

	bool init() override;
	void update(float dt) override;

	void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture);
	void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture);
	void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture);
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture);

	void setSpriteDirection(SpriteDirection_t direction);
	void setForwardDirection();
	void setBackwardDirection();

	void runAnimation(int code);
	void runAnimation(int code, int startFrameIndexInAnimation);
	void pauseAnimation();
	void pauseAnimation(float delay);
	void resumeAnimation();

	ActorPartSprite* getBodyPart();
	Actor* getActor() { return m_pActor; }
	SGNode* getBodyCanvas();
	SGSize	getBodyCanvasSize();
	SGSize	getBodyPartSize();
	SGVec2	getBodyPartPosition();
	PartData createPart(const ActorPartSpriteData& partSpriteData, int frameCount);

	SGVector<PartData>& getParts() { return m_vParts; }
	SpriteDirection_t getSpriteDirection();
	int getRunningAnimationCode();
	ActorPartAnimation* getRunningAnimation();
	void updateSpriteData(const SGActorSpriteDataPtr& spriteData);
private:

	int m_iFrameCount;
	Actor* m_pActor;
	SGActorSpriteDataPtr m_spActorData;
	SGVector<PartData> m_vParts;
	//SGVector<SGSprite*> m_vPartsCanvas;
	//SGVector<SGActorPartSprite*> m_vParts;
	//SGVector<SGDrawNode*> m_vPartsBoundingBox;
	SpriteDirection_t m_eDirection;
};


