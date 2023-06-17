/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:34:00 AM
 * =====================
 *
 */


#pragma once

NS_JC_BEGIN
	class Scheduler;
	class ThreadPool;
NS_JC_END

class CommonInfo;
struct CharCommonInfo;
struct ServerProcessInfoPackage;

extern CommonInfo* CoreCommonInfo_v;
extern CharCommonInfo* CoreCharCommon_v;
extern ServerProcessInfoPackage* CoreServerProcessInfoPackage_v;
extern JCore::Scheduler* CoreScheduler_v;
extern JCore::ThreadPool* CoreThreadPool_v;

void InitializeCommonCore();
void FinalizeCommonCore();