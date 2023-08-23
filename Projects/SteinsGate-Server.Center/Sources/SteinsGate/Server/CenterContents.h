/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 1:52:29 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ServerContents.h>
#include <SteinsGate/Server/BootStateManager.h>

struct CenterContents : ServerContents
{
	::BootStateManager* BootStateManager;

	void Initialize() override;
	void Finalize() override;
};