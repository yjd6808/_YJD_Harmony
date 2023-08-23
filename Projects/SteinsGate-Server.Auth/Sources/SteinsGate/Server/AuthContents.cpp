/*
 * 작성자: 윤정도
 * 생성일: 6/15/2023 1:53:02 AM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthenticationManager.h"

#include <SteinsGate/Server/AuthenticationManagerSchedule.h>

USING_NS_JC;

void IntializeScheduler();
void FinalizeScheduler();


AuthenticationManagerSchedule* g_pAuthTokenManagerSchedule;

void InitializeAuthContents() {
	IntializeScheduler();
}

void FinalizeAuthContents() {
	FinalizeScheduler();
}


void IntializeScheduler() {
	g_pAuthTokenManagerSchedule = dbg_new AuthenticationManagerSchedule();

	g_pAuthTokenManagerSchedule->Initialize();

	Core::Scheduler->AddFirstTask(g_pAuthTokenManagerSchedule);
}

void FinalizeScheduler() {
	g_pAuthTokenManagerSchedule->Finalize();
}