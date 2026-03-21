/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:40:57 AM
 * =====================
 *
 */

#pragma once

#include "sgcl/GunnerAction.h"

class GunnerSliding : public GunnerAction
{
public:
	GunnerSliding(HostPlayer* _pPlayer, ActionInfo* _pActionInfo);

	bool OnConditionCheck() override;
	void OnUpdate(float _deltaTime) override;
	void OnActionBegin() override;
	void OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) override;
	void OnEnemySingleHit(HitInfo& _info);
	void OnEnemyMultiHit(SGHitInfoList& _hitList, int _newHitCount);

private:
	bool slidingStarted_{};
};
