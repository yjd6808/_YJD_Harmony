/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:34:00 AM
 * =====================
 *
 */


#pragma once

#include <sg/Namepsace.h>

NS_JC_BEGIN
class Scheduler;
class ThreadPool;
NS_JC_END

NS_JNET_BEGIN
class CommandNameDictionary;
NS_JNET_END

class DescLoaderMgr;
struct CharCommonInfo;
struct ServerProcessInfoPackage;
struct RuntimeConfig;


NS_SG_BEGIN
extern CharCommonInfo* CharCommon;
extern ServerProcessInfoPackage* ServerProcessInfoPackage;
extern jnet::CommandNameDictionary CommandNameDictionary;
extern DescLoaderMgr* DescLoaderMgr;
NS_SG_END

void InitializeCommonCore();
void FinalizeCommonCore();
