/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 4:24:01 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCore.h"

#include <SteinsGate/Client/SGDataManager.h>

// ===========================================================
//     슈타인즈 게이트 모든 세계션이 만나는 곳
// ===========================================================
SGClientInfo* CoreInfo_v;

void InitializeClientCore() {
	CoreInfo_v = SGDataManager::get()->getClientInfo(1);
}

void FinalizeClientCore() {
	
}