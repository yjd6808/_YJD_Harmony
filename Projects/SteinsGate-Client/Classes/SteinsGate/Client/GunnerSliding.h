/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/GunnerAction.h>

class GunnerSliding : public GunnerAction {
public:
	GunnerSliding(HostPlayer* player, ActionInfo* actionInfo);

	bool onConditionCheck() override;
	void onUpdate(float dt) override;
	void onActionBegin() override;
	void onFrameEnd(ActorPartAnimation* animation, FrameTexture* frame) override;
	void onEnemySingleHit(HitInfo& info);
	void onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount);
private:
	bool m_bSlidingStarted{};
	
};

