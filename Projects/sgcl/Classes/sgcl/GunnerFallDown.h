/*
 * 
 * 
 * =====================
 *
 */

#pragma once

#include <sgcl/GunnerAction.h>

class GunnerFallDown : public GunnerAction
{
public:
	GunnerFallDown(HostPlayer* _pPlayer, ActionInfo* _pActionInfo);

	void OnActionBegin() override;
	void OnActionEnd() override;
	void OnUpdate(float _deltaTime) override;

private:
	bool bounced_;
	bool down_;
	float elapsedDownTime_;
	float downRecoverTime_;
};
