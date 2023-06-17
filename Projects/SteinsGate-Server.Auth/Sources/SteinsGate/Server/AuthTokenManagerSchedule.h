/*
 * 작성자: 윤정도
 * 생성일: 6/15/2023 1:13:03 AM
 * =====================
 *
 */


#pragma once

 #include <JCore/Threading/Scheduler.h>

class AuthTokenManagerSchedule : public JCore::SchedulerTaskRunnableRepeat
{
public:
	void OnFirstScheduled(SchedulerTask* task) override;
	void OnScheduled(SchedulerTask* task) override;

	void Initialize();
	void Finalize();

	JCore::TimeSpan Interval() override { return JCore::TimeSpan::FromMiliSeocnd(994); }
};

