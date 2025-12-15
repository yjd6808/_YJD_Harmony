/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 3:47:00 PM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/Actor.h>
#include <SteinsGate/Client/EffectInfo.h>

class Effect : public Actor
{
public:
	Effect(EffectInfo* _pBaseInfo);
	~Effect() override;

	static Effect* create(EffectInfo* _pBaseInfo);

	void initialize() override;
	bool initVariables() override;
	void initActorSprite() override;
	void initListeners() override;
	void initComponents() override;

	EffectInfo* getBaseInfo() { return baseInfo_; }
	ActorType_t getType() const override { return ActorType::Effect; }
	int getCode() override { return baseInfo_->code_; }

	void update(float _delta) override;
	void onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;

private:
	EffectInfo* baseInfo_;
};
