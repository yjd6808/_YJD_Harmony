/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 3:47:00 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/SGActor.h>
#include <SteinsGate/Research/SGEffectInfo.h>

class SGEffect : public SGActor
{
public:
	SGEffect(SGEffectInfo* baseInfo);
	~SGEffect() override;

	static SGEffect* create(SGEffectInfo* baseInfo);

	bool initVariables() override;
	void initActorSprite() override;
	void initListener(SGActorListener* listener) override;
	
	SGEffectInfo* getBaseInfo() { return m_pBaseInfo; }

	void update(float delta) override;
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;

private:
	SGEffectInfo* m_pBaseInfo;
};