/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:34:00 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Namespace.h>
#include <JNetwork/Namespace.h>

NS_JC_BEGIN
	class Scheduler;
	class ThreadPool;
NS_JC_END

NS_JNET_BEGIN
	class CommandNameDictionary;
NS_JNET_END

class CLIThread;
class CommonInfo;
struct CharCommonInfo;
struct ServerProcessInfoPackage;
struct RuntimeConfigBase;

extern CLIThread* CoreCLIThread_v;
extern CommonInfo* CoreCommonInfo_v;
extern CharCommonInfo* CoreCharCommon_v;
extern ServerProcessInfoPackage* CoreServerProcessInfoPackage_v;
extern JCore::Scheduler* CoreScheduler_v;
extern JCore::ThreadPool* CoreThreadPool_v;
extern JNetwork::CommandNameDictionary CoreCommandNameDictionary_v;
extern RuntimeConfigBase* CoreRuntimeConfigBase_v;


void InitializeCommonCore();
void FinalizeCommonCore();