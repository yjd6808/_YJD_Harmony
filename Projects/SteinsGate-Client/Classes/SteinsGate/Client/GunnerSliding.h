/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/GunnerAction.h>

class GunnerSliding : public GunnerAction
{
public:
	GunnerSliding(HostPlayer* _pPlayer, ActionInfo* _pActionInfo);

	bool onConditionCheck() override;
	void onUpdate(float _deltaTime) override;
	void onActionBegin() override;
	void onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void onEnemySingleHit(HitInfo& _info);
	void onEnemyMultiHit(SGHitInfoList& _hitList, int _newHitCount);

private:
	bool slidingStarted_{};
};
