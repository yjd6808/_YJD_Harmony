/*
 * 작성자: 윤정도
 * 생성일: 6/15/2023 1:53:02 AM
 * =====================
 *
 */


#include "Center.h"
#include "CenterCoreHeader.h"
#include "CenterContents.h"

USING_NS_JC;

void CenterContents::Initialize() {
	BootStateManager = BootStateManager::Get();
}

void CenterContents::Finalize() {
	ClearSchedule();

	JCORE_DELETE_SINGLETON_SAFE(BootStateManager);
}
