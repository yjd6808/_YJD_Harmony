/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:11 PM
 * =====================
 * 파츠의 0번 인덱스는 무조건 바디 파츠로 고정
 *
 */

#pragma once

#include <SteinsGate/Client/Tutturu.h>
#include <SteinsGate/Client/ActorPartAnimation.h>

class ActorSprite;

class ActorPartSprite : public SGSprite
{
public:
	static ActorPartSprite* create(
		int _frameCount,
		ActorSprite* _pActor,
		SGNode* _pCanvas,
		SGDrawNode* _pBoundingBox,
		const ActorPartSpriteData& _partData,
		SGVector<AnimationInfo>* _pAnimations
	);

	ActorPartSprite(
		int _frameCount,
		ActorSprite* _pActor,
		SGNode* _pCanvas,
		SGDrawNode* _pBoundingBox,
		const ActorPartSpriteData& _partData,
		SGVector<AnimationInfo>* _pAnimations
	);
	~ActorPartSprite() override;

	bool init() override;
	void initFrames();
	void initAnimations();

	void update(float _dt) override;
	void updateBoundingBoxVisibleState();
	void updateAnimationReference(SGVector<AnimationInfo>* _pAnimations);

	void runAnimation(int _code);
	void runAnimation(int _code, int _startFrameIndexInAnimation);

	void setPartIndex(int _index)
	{
		partIndex_ = _index;
	}

	int getPartIndex()
	{
		return partIndex_;
	}

	ActorType_t getActorType();

	SGNode* getCanvas()
	{
		return canvas_;
	}

	SGDrawNode* getPartBoundingBox()
	{
		return boundingBox_;
	}

	ActorSprite* getActorSprite()
	{
		return actorSprite_;
	}

	const ActorPartSpriteData& getPartData()
	{
		return partData_;
	}

	ActorPartAnimation* getRunningAnimation()
	{
		return runningAnimation_;
	}

	void onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onCustomFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onCustomFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void reflectAnimation(ActorPartAnimation* _pRunningAnimation);

private:
	// 주입 데이터
	int partIndex_;	// 몇번째 파츠인지
	ActorSprite* actorSprite_;
	ActorPartSpriteData partData_;
	SGVector<AnimationInfo>* animationInfoList_;

	// 자체 데이터
	SGNode* canvas_;
	SGDrawNode* boundingBox_;
	ActorPartAnimation* runningAnimation_;
	SGHashMap<int, ActorPartAnimation*> animationMap_;
	SGVector<ActorPartAnimation*> animationList_;
	SGVector<FrameTexture*> frames_;	// 애니메이션 전체 프레임
};
