/*
 * 작성자: 윤정도
 * 생성일: 6/17/2023 11:24:28 PM
 * =====================
 *
 */


#pragma once

#include "sg/Net/NetListenerHelper.h"

NS_SG_BEGIN

struct ListenerHelperCommon : NetListenerHelper
{
	static bool IsRelayCommand(jnet::ICommand* _pCommand);
	static bool IsHostCommand(jnet::ICommand* _pCommand);
	static bool IsStaticCommand(jnet::ICommand* _pCommand);
	static bool IsDynamicCommand(jnet::ICommand* _pCommand);
	static bool IsInterServerCommand(jnet::ICommand* _pCommand);
};

NS_END