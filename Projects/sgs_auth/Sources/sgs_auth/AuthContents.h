/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 1:52:29 PM
 * =====================
 *
 */


#pragma once

#include <sgs/ServerContents.h>
#include <sgs_auth/AuthenticationManager.h>

struct AuthContents : ServerContents
{
	::AuthenticationManager* AuthenticationManager;

	void Initialize() override;
	void Finalize() override;
};