/*
 * 작성자: 윤정도
 * 생성일: 6/15/2023 1:48:33 AM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthenticationManagerSchedule.h"

USING_NS_JC;

void AuthenticationManagerSchedule::Initialize() {

}

void AuthenticationManagerSchedule::Finalize() {
}

void AuthenticationManagerSchedule::OnFirstScheduled(SchedulerTask* task) {
	JCORE_PASS;
}

void AuthenticationManagerSchedule::OnScheduled(SchedulerTask* task) {
	AuthenticationManager::Get()->OnScheduled(task);
}


