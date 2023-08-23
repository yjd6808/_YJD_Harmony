/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:34:00 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Namepsace.h>

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

NS_CORE_BEGIN
extern CLIThread* CLIThread;
extern CommonInfo* CommonInfo;
extern CharCommonInfo* CharCommon;
extern ServerProcessInfoPackage* ServerProcessInfoPackage;
extern JCore::Scheduler* Scheduler;
extern JCore::ThreadPool* ThreadPool;
extern JNetwork::CommandNameDictionary CommandNameDictionary;
extern RuntimeConfigBase* RuntimeConfigBase;
NS_CORE_END


void InitializeCommonCore();
void FinalizeCommonCore();