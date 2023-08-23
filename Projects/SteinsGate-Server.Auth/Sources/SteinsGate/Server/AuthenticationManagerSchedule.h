/*
 * 작성자: 윤정도
 * 생성일: 6/15/2023 1:13:03 AM
 * =====================
 *
 */


#pragma once

 #include <JCore/Threading/SchedulerTask.h>

class AuthenticationManagerSchedule : public JCore::SchedulerTaskRunnable
{
public:
	void OnFirstScheduled() override;
	void OnScheduled() override;

	void Initialize() override;
	void Finalize() override;

	JCore::TimeSpan Interval() override { return JCore::TimeSpan::FromMiliSeocnd(994); }
};

