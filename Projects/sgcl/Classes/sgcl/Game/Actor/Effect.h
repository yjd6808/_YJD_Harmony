/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 3:47:00 PM
 * =====================
 *
 */

#pragma once

#include "sgcl/Game/Actor/Actor.h"
#include "sg/Struct/SteinsGate_Effect.h"

class Effect : public Actor
{
public:
	Effect(EffectInfo* _pBaseInfo);
	~Effect() override;

	static Effect* Create(EffectInfo* _pBaseInfo);

	void Initialize() override;
	bool InitVariables() override;
	void InitActorSprite() override;
	void InitListeners() override;
	void InitComponents() override;

	EffectInfo* GetBaseInfo() { return pBaseInfo_; }
	ActorType_t GetType() const override { return ActorType::Effect; }
	int GetCode() override { return pBaseInfo_->code_; }

	void update(float _delta) override;
	void OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;

private:
	EffectInfo* pBaseInfo_;
};
