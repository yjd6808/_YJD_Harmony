/*
 * 작성자: 윤정도
 * 생성일: 6/15/2023 1:48:33 AM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthTokenManagerSchedule.h"

USING_NS_JC;

void AuthTokenManagerSchedule::Initialize() {

}

void AuthTokenManagerSchedule::Finalize() {
}

void AuthTokenManagerSchedule::OnFirstScheduled(SchedulerTask* task) {
	JCORE_PASS;
}

// static DateTime s = DateTime::Now();
void AuthTokenManagerSchedule::OnScheduled(SchedulerTask* task) {
	// Console::WriteLine("%.2f초 경과", DateTime::Now().Diff(s).GetTotalSeconds());
}


