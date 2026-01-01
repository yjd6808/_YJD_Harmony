/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 1:52:29 PM
 * =====================
 *
 */


#pragma once

#include <sgs/ServerContents.h>
#include <sgs_center/BootStateManager.h>

struct CenterContents : ServerContents
{
	::BootStateManager* BootStateManager;

	void Initialize() override;
	void Finalize() override;
};
