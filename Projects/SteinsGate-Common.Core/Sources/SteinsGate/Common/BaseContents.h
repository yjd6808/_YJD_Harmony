/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 3:38:17 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Threading/SchedulerTask.h>

struct BaseContents
{
protected:
	virtual ~BaseContents();

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

	void AddSchedule(JCore::SchedulerTaskRunnable* schedule);
	void ClearSchedule();
private:
	SGVector<JCore::SchedulerTaskRunnable*> m_vSchedules;
};



