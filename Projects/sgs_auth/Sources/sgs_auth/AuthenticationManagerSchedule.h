/*
 * 작성자: 윤정도
 * 생성일: 6/15/2023 1:13:03 AM
 * =====================
 *
 */

#pragma once

#include <jc/Threading/SchedulerTask.h>

class AuthenticationManagerSchedule : public jc::SchedulerTaskRunnable
{
public:
	void OnFirstScheduled() override;
	void OnScheduled() override;

	void Initialize() override;
	void Finalize() override;

	jc::TimeSpan Interval() override
	{
		return jc::TimeSpan::FromMiliSeocnd(994);
	}
};
