/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:35 PM
 * =====================
 *
 */


#pragma once

#include <sgcl/Core.h>
#include <sgcl/ActorPartSprite.h>


class Actor;

class ActorSprite : public cc::Sprite
{
public:
	struct PartData
	{
		cc::Sprite* canvas_;
		ActorPartSprite* pPart_;
		cc::DrawNode* boundingBox_;

		bool operator==(const PartData& _other);
	};

	ActorSprite(Actor* _pActor, ActorSpriteData* _pActorSpriteData);
	static ActorSprite* Create(Actor* _pActor, ActorSpriteData* _pActorSpriteData);

	bool init() override;
	void update(float _deltaTime) override;

	void OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);

	void SetSpriteDirection(SpriteDirection_t _direction);
	void SetForwardDirection();
	void SetBackwardDirection();

	void RunAnimation(int _code);
	void RunAnimation(int _code, int _startFrameIndexInAnimation);
	void PauseAnimation();
	void PauseAnimation(float _delay);
	void ResumeAnimation();

	ActorPartSprite* GetBodyPart();
	Actor* GetActor() { return pActor_; }
	cc::Node* GetBodyCanvas();
	cc::size GetBodyCanvasSize();
	cc::size GetBodyPartSize();
	cc::vec2 GetBodyPartPosition();
	PartData CreatePart(const ActorPartSpriteData& _partSpriteData, int _frameCount);

	jc::Vector<PartData>& GetParts() { return parts_; }
	SpriteDirection_t GetSpriteDirection();
	int GetRunningAnimationCode();
	ActorPartAnimation* GetRunningAnimation() const;
	void UpdateSpriteData(ActorSpriteData* _pSpriteData);

private:
	int frameCount_;
	Actor* pActor_;
	ActorSpriteData* pActorSpriteData_;
	jc::Vector<PartData> parts_;
	SpriteDirection_t direction_;
};
