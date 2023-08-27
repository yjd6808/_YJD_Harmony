/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 2:49:14 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/BaseContents.h>
#include <SteinsGate/Common/ServerType.h>

struct ServerContents : BaseContents
{
	::UnauthenticatedSessionManager* UnauthenticatedSessionManager;

	void Initialize() override;
	void Finalize() override;
};

