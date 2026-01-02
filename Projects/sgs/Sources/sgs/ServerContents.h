/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 2:49:14 PM
 * =====================
 *
 */


#pragma once

#include <sg/BaseContents.h>

class UnauthenticatedSessionManager;

struct ServerContents : BaseContents
{
	::UnauthenticatedSessionManager* UnauthenticatedSessionManager;

	void Initialize() override;
	void Finalize() override;
};
