/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:34:00 AM
 * =====================
 *
 */


#pragma once

namespace JCore {
	class Scheduler;
	class ThreadPool;
}

class CommonInfo;
struct CharCommonInfo;
struct ServerProcessInfo;


extern CommonInfo* CoreCommonInfo_v;
extern CharCommonInfo* CoreCharCommon_v;
extern ServerProcessInfo* CoreServerProcessInfo_v;
extern JCore::Scheduler* CoreScheduler_v;
extern JCore::ThreadPool* CoreThreadPool_v;

void InitializeCommonCore();
void FinalizeCommonCore();