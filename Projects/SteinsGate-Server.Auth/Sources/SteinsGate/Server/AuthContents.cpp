/*
 * 작성자: 윤정도
 * 생성일: 6/15/2023 1:53:02 AM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthTokenManager.h"

#include <SteinsGate/Server/AuthTokenManagerSchedule.h>

USING_NS_JC;

void IntializeScheduler();
void FinalizeScheduler();

void InitializeAuthContents() {
	IntializeScheduler();
}

void FinalizeAuthContents() {
	FinalizeScheduler();
}


void IntializeScheduler() {
	CoreScheduler_v->AddFirstTask(dbg_new AuthTokenManagerSchedule());
}

void FinalizeScheduler() {
}