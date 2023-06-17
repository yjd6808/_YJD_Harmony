/*
 * 작성자: 윤정도
 * 생성일: 6/17/2023 11:26:26 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "ListenerHelper.h"

#include <SteinsGate/Common/InterServerCmdDefine.h>

USING_NS_JC;
USING_NS_JNET;

bool ListenerHelper::IsRelayCommand(ICommand* cmd) {
	const CmdType_t type = cmd->Type;
	if (type == InterServerCmdType::RelayStatic)
		return true;
	if (type == InterServerCmdType::RelayDynamic)
		return true;
	return false;
}


bool ListenerHelper::IsHostCommand(ICommand* cmd) {
	const CmdType_t type = cmd->Type;
	if (type == InterServerCmdType::HostStatic)
		return true;
	if (type == InterServerCmdType::HostDynamic)
		return true;
	return false;
}

bool ListenerHelper::IsStaticCommand(ICommand* cmd) {
	const CmdType_t type = cmd->Type;
	if (type == InterServerCmdType::HostStatic)
		return true;
	if (type == InterServerCmdType::RelayStatic)
		return true;
	if (type == InterServerCmdType::Static)
		return true;
	return false;
}

bool ListenerHelper::IsDynamicCommand(ICommand* cmd) {
	const CmdType_t type = cmd->Type;
	if (type == InterServerCmdType::HostDynamic)
		return true;
	if (type == InterServerCmdType::RelayDynamic)
		return true;
	if (type == InterServerCmdType::Dynamic)
		return true;
	return false;
}

bool ListenerHelper::IsInterServerCommand(JNetwork::ICommand* cmd) {
	const CmdType_t type = cmd->Type;
	return type >= InterServerCmdType::InterServerBegin && type <= InterServerCmdType::InterServerEnd;
}
