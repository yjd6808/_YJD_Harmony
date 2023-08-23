/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 3:39:15 PM
 * =====================
 *
 */

#include "Core.h"
#include "CommonCoreHeader.h"
#include "BaseContents.h"

BaseContents::~BaseContents() {
	DebugAssertMsg(m_vSchedules.Size() == 0, "아직 삭제되지 않은 스케쥴들이 있습니다. (수동으로 스케쥴들을 정리해주세요.)");
}

void BaseContents::AddSchedule(JCore::SchedulerTaskRunnable* schedule) {

	if (!Core::Scheduler) {
		_LogWarn_("스케쥴러가 설정되어있지 않습니다.");
		delete schedule;
		return;
	}

	schedule->Initialize();
	m_vSchedules.PushBack(schedule); 
	Core::Scheduler->AddFirstTask(schedule);
}

void BaseContents::ClearSchedule() {
	for (int i = 0; i < m_vSchedules.Size(); ++i) {
		m_vSchedules[i]->Finalize();
		// 메모리해제는 스케쥴러가 알아서해줌.
	}
	m_vSchedules.Clear();
}


