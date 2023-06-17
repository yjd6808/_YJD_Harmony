/*
 * 작성자: 윤정도
 * 생성일: 6/17/2023 11:24:28 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet/Command.h>

class ListenerHelper
{
protected:
	static bool IsRelayCommand(JNetwork::ICommand* cmd);
	static bool IsHostCommand(JNetwork::ICommand* cmd);
	static bool IsStaticCommand(JNetwork::ICommand* cmd);
	static bool IsDynamicCommand(JNetwork::ICommand* cmd);
	static bool IsInterServerCommand(JNetwork::ICommand* cmd);
};
