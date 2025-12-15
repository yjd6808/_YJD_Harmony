/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 7:02:18 PM
 * =====================
 * 쓰러진 상태 회복전 중간단계 앉아 있기
 */

#pragma once

#include <SteinsGate/Client/GunnerAction.h>

class GunnerSitRecover : public GunnerAction
{
public:
	GunnerSitRecover(HostPlayer* _pPlayer, ActionInfo* _pActionInfo);

	void onActionBegin() override;
	void onUpdate(float _dt) override;

private:
	float elapsedSitTime_;
	float recoverSitTime_;
};
