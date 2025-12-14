/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:35 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>
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

		bool operator==(const PartData& _other);
	};

	ActorSprite(Actor* _pActor, ActorSpriteData* _pActorSpriteData);
	static ActorSprite* create(Actor* _pActor, ActorSpriteData* _pActorSpriteData);

	bool init() override;
	void update(float _dt) override;

	void onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);

	void setSpriteDirection(SpriteDirection_t _direction);
	void setForwardDirection();
	void setBackwardDirection();

	void runAnimation(int _code);
	void runAnimation(int _code, int _startFrameIndexInAnimation);
	void pauseAnimation();
	void pauseAnimation(float _delay);
	void resumeAnimation();

	ActorPartSprite* getBodyPart();
	Actor* getActor() { return actor_; }
	SGNode* getBodyCanvas();
	SGSize getBodyCanvasSize();
	SGSize getBodyPartSize();
	SGVec2 getBodyPartPosition();
	PartData createPart(const ActorPartSpriteData& _partSpriteData, int _frameCount);

	SGVector<PartData>& getParts() { return parts_; }
	SpriteDirection_t getSpriteDirection();
	int getRunningAnimationCode();
	ActorPartAnimation* getRunningAnimation() const;
	void updateSpriteData(ActorSpriteData* _pSpriteData);
private:
	int frameCount_;
	Actor* actor_;
	ActorSpriteData* actorData_;
	SGVector<PartData> parts_;
	SpriteDirection_t direction_;
};


