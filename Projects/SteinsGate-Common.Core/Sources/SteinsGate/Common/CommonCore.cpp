/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:35:17 AM
 * =====================
 *
 */

#include "Core.h"
#include "CommonCore.h"
#include "CommonCoreHeader.h"

USING_NS_JC;

ServerProcessInfo* CoreServerProcessInfo_v;			// 메인 서버 프로그램에서 주입해줄 것
CommonInfo* CoreCommonInfo_v = nullptr;				// 메인 프로그램에서 주입해줄 것
CharCommonInfo* CoreCharCommon_v = nullptr;			// 메인 프로그램에서 주입해줄 것
ThreadPool* CoreThreadPool_v = nullptr;				// 메인 프로그램에서 주입해줄 것
Scheduler* CoreScheduler_v = nullptr;				// 메인 프로그램에서 주입해줄 것

void InitializeCommonCore() {

}

void FinalizeCommonCore() {
}