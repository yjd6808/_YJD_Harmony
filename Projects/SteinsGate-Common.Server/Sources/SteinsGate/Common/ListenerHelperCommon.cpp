/*
 * 작성자: 윤정도
 * 생성일: 6/17/2023 11:26:26 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "ListenerHelperCommon.h"

#include <SteinsGate/Common/InterServerCmdDefine.h>

USING_NS_JC;
USING_NS_JNET;

bool ListenerHelperCommon::IsRelayCommand(ICommand* cmd) {
	const CmdType_t type = cmd->Type;
	if (type == InterServerCmdType::RelayStatic)
		return true;
	if (type == InterServerCmdType::RelayDynamic)
		return true;
	return false;
}


bool ListenerHelperCommon::IsHostCommand(ICommand* cmd) {
	const CmdType_t type = cmd->Type;
	if (type == InterServerCmdType::HostStatic)
		return true;
	if (type == InterServerCmdType::HostDynamic)
		return true;
	return false;
}

bool ListenerHelperCommon::IsStaticCommand(ICommand* cmd) {
	const CmdType_t type = cmd->Type;
	if (type == InterServerCmdType::HostStatic)
		return true;
	if (type == InterServerCmdType::RelayStatic)
		return true;
	if (type == InterServerCmdType::Static)
		return true;
	return false;
}

bool ListenerHelperCommon::IsDynamicCommand(ICommand* cmd) {
	const CmdType_t type = cmd->Type;
	if (type == InterServerCmdType::HostDynamic)
		return true;
	if (type == InterServerCmdType::RelayDynamic)
		return true;
	if (type == InterServerCmdType::Dynamic)
		return true;
	return false;
}

bool ListenerHelperCommon::IsInterServerCommand(JNetwork::ICommand* cmd) {
	const CmdType_t type = cmd->Type;
	return type >= InterServerCmdType::InterServerBegin && type <= InterServerCmdType::InterServerEnd;
}
