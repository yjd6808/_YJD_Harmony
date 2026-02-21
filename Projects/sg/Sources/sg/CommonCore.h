/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:34:00 AM
 * =====================
 *
 */


#pragma once

#include <sg/Namespace.h>

extern SG_DLL jc::Random __globalRandomInstance;
#define g_cRandom __globalRandomInstance

NS_JNET_BEGIN
class CommandNameDictionary;
NS_END

struct CharCommonInfo;

NS_SG_BEGIN
extern CharCommonInfo* CharCommon;
extern jnet::CommandNameDictionary CommandNameDictionary;
NS_END

void InitializeCommonCore();
void FinalizeCommonCore();
