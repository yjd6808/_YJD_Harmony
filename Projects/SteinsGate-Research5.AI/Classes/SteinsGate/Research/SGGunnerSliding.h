/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Research/SGGunnerAction.h>

class SGGunnerSliding : public SGGunnerAction {
public:
	SGGunnerSliding(SGPlayer* player, SGActionInfo* actionInfo);

	void onUpdate(float dt) override;
	void onActionBegin() override;
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
private:
	bool m_bSlidingStarted{};
};

