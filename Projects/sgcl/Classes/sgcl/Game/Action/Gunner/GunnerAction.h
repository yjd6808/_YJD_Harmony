/*
 * 작성자: 윤정도
 * 생성일: 1/28/2023 8:14:43 AM
 * =====================
 *
 */

#pragma once


#include "sgcl/Game/Action/SGAction.h"
#include "sg/Struct/SteinsGate_Char.h"

class GunnerAction : public SGAction
{
public:
	GunnerAction(HostPlayer* _pHostPlayer, ActionInfo* _pActionInfo);
	~GunnerAction() override;

	void Play() override;

	bool OnConditionCheck() override
	{
		return true;
	}

protected:
	GunnerInfo* pBaseInfo_;
};
