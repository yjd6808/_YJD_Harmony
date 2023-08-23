/*
 * 작성자: 윤정도
 * 생성일: 6/15/2023 1:53:02 AM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthContents.h"

#include <SteinsGate/Server/AuthenticationManagerSchedule.h>

USING_NS_JC;

void AuthContents::Initialize() {
	AuthenticationManager = AuthenticationManager::Get();

	AddSchedule(dbg_new AuthenticationManagerSchedule);
}

void AuthContents::Finalize() {
	ClearSchedule();

	JCORE_DELETE_SINGLETON_SAFE(AuthenticationManager);
}
