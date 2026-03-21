/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:11 PM
 * =====================
 * 파츠의 0번 인덱스는 무조건 바디 파츠로 고정
 *
 */

#pragma once

#include "sgcl/ActorPartAnimation.h"

class ActorSprite;

class ActorPartSprite : public cc::Sprite
{
public:
	static ActorPartSprite* Create(
		int _frameCount,
		ActorSprite* _pActor,
		cc::Node* _pCanvas,
		cc::DrawNode* _pBoundingBox,
		const ActorPartSpriteData& _partData,
		jc::Vector<AnimationInfo>* _pAnimations);

	ActorPartSprite(
		int _frameCount,
		ActorSprite* _pActor,
		cc::Node* _pCanvas,
		cc::DrawNode* _pBoundingBox,
		const ActorPartSpriteData& _partData,
		jc::Vector<AnimationInfo>* _pAnimations);

	~ActorPartSprite() override;

	bool init() override;
	void InitFrames();
	void InitAnimations();

	void update(float _dt) override;
	void UpdateBoundingBoxVisibleState();
	void UpdateAnimationReference(jc::Vector<AnimationInfo>* _pAnimations);

	void RunAnimation(int _code);
	void RunAnimation(int _code, int _startFrameIndexInAnimation);

	void SetPartIndex(int _index) { partIndex_ = _index; }
	int GetPartIndex() { return partIndex_; }
	ActorType_t GetActorType();
	cc::Node* GetCanvas() { return canvas_; }
	cc::DrawNode* GetPartBoundingBox() { return boundingBox_; }
	ActorSprite* GetActorSprite() { return pActorSprite_; }
	const ActorPartSpriteData& GetPartData() { return partData_; }
	ActorPartAnimation* GetRunningAnimation() { return runningAnimation_; }

	void OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnCustomFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnCustomFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void ReflectAnimation(ActorPartAnimation* _pRunningAnimation);

private:
	// 주입 데이터
	int partIndex_;	// 몇번째 파츠인지
	ActorSprite* pActorSprite_;
	ActorPartSpriteData partData_;
	jc::Vector<AnimationInfo>* pAnimationInfoList_;

	// 자체 데이터
	cc::Node* canvas_;
	cc::DrawNode* boundingBox_;
	ActorPartAnimation* runningAnimation_;
	jc::HashMap<int, ActorPartAnimation*> animationMap_;
	jc::Vector<ActorPartAnimation*> animationList_;
	jc::Vector<FrameTexture*> frames_;	// 애니메이션 전체 프레임
};
