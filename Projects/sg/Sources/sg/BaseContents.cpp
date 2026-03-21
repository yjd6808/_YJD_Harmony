/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 3:39:15 PM
 * =====================
 *
 */

#include "BaseContents.h"

//////////////////////////////////////////////////////////////////////////////////////////
BaseContents::~BaseContents()
{
	jc_assert_msg(schedules_.Size() == 0, "아직 삭제되지 않은 스케쥴들이 있습니다. (수동으로 스케쥴들을 정리해주세요.)");
}

//////////////////////////////////////////////////////////////////////////////////////////
void BaseContents::AddSchedule(jc::SchedulerTaskRunnable* _pSchedule)
{
	_pSchedule->Initialize();
	schedules_.PushBack(_pSchedule);
	g_cScheduler.AddFirstTask(_pSchedule);
}

//////////////////////////////////////////////////////////////////////////////////////////
void BaseContents::ClearSchedule()
{
	for (int scheduleIndex = 0; scheduleIndex < schedules_.Size(); ++scheduleIndex)
	{
		schedules_[scheduleIndex]->Finalize();
		// 메모리해제는 스케쥴러가 알아서해줌.
	}

	schedules_.Clear();
}
