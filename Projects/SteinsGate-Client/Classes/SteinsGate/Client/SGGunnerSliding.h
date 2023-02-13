/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/SGGunnerAction.h>

class SGGunnerSliding : public SGGunnerAction {
public:
	SGGunnerSliding(SGHostPlayer* player, SGActionInfo* actionInfo);

	bool onConditionCheck() override;
	void onUpdate(float dt) override;
	void onActionBegin() override;
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) override;
	void onEnemySingleHit(SGHitInfo& info);
	void onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount);
private:
	bool m_bSlidingStarted{};
	
};

