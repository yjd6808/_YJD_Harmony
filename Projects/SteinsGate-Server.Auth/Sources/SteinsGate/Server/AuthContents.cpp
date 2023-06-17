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


AuthTokenManagerSchedule* g_pAuthTokenManagerSchedule;

void InitializeAuthContents() {
	IntializeScheduler();
}

void FinalizeAuthContents() {
	FinalizeScheduler();
}


void IntializeScheduler() {
	g_pAuthTokenManagerSchedule = dbg_new AuthTokenManagerSchedule();

	g_pAuthTokenManagerSchedule->Initialize();

	CoreScheduler_v->AddFirstTask(g_pAuthTokenManagerSchedule);
}

void FinalizeScheduler() {
	g_pAuthTokenManagerSchedule->Finalize();
}