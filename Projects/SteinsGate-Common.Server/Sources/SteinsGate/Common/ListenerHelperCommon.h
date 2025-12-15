/*
 * 작성자: 윤정도
 * 생성일: 6/17/2023 11:24:28 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ListenerHelperBase.h>

struct ListenerHelperCommon : ListenerHelperBase
{
	static bool IsRelayCommand(JNetwork::ICommand* _pCommand);
	static bool IsHostCommand(JNetwork::ICommand* _pCommand);
	static bool IsStaticCommand(JNetwork::ICommand* _pCommand);
	static bool IsDynamicCommand(JNetwork::ICommand* _pCommand);
	static bool IsInterServerCommand(JNetwork::ICommand* _pCommand);
};
