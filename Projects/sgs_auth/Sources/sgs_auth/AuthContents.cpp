/*
 * 작성자: 윤정도
 * 생성일: 6/15/2023 1:53:02 AM
 * =====================
 *
 */


#include "Core.h"
#include "AuthCoreHeader.h"
#include "AuthContents.h"

#include <sgs_auth/AuthenticationManagerSchedule.h>

USING_NS_JC;

void AuthContents::Initialize() {
	AuthenticationManager = AuthenticationManager::Get();

	AddSchedule(dbg_new AuthenticationManagerSchedule);
}

void AuthContents::Finalize() {
	ClearSchedule();

	JC_DELETE_SINGLETON_SAFE(AuthenticationManager);
}
