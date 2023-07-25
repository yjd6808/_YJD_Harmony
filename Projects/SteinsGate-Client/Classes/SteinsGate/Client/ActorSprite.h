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

	ActorSprite(Actor* actor, ActorSpriteData* actorSpriteData);
	static ActorSprite* create(Actor* actor, ActorSpriteData* actorSpriteData);

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
	void updateSpriteData(ActorSpriteData* spriteData);
private:
	int m_iFrameCount;
	Actor* m_pActor;
	ActorSpriteData* m_pActorData;
	SGVector<PartData> m_vParts;
	SpriteDirection_t m_eDirection;
};


