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
	Effect(EffectInfo* baseInfo);
	~Effect() override;

	static Effect* create(EffectInfo* baseInfo);

	bool initVariables() override;
	void initActorSprite() override;
	
	EffectInfo* getBaseInfo() { return m_pBaseInfo; }
	int getCode() override { return m_pBaseInfo->Code; }

	void update(float delta) override;
	void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) override;

private:
	EffectInfo* m_pBaseInfo;
};