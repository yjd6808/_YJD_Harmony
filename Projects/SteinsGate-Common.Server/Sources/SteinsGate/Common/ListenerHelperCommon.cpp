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

//////////////////////////////////////////////////////////////////////////////////////////
bool ListenerHelperCommon::IsRelayCommand(ICommand* _pCommand)
{
	const CmdType_t type = _pCommand->GetType();
	if (type == InterServerCmdType::RelayStatic)
		return true;
	if (type == InterServerCmdType::RelayDynamic)
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ListenerHelperCommon::IsHostCommand(ICommand* _pCommand)
{
	const CmdType_t type = _pCommand->GetType();
	if (type == InterServerCmdType::HostStatic)
		return true;
	if (type == InterServerCmdType::HostDynamic)
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ListenerHelperCommon::IsStaticCommand(ICommand* _pCommand)
{
	const CmdType_t type = _pCommand->GetType();
	if (type == InterServerCmdType::HostStatic)
		return true;
	if (type == InterServerCmdType::RelayStatic)
		return true;
	if (type == InterServerCmdType::Static)
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ListenerHelperCommon::IsDynamicCommand(ICommand* _pCommand)
{
	const CmdType_t type = _pCommand->GetType();
	if (type == InterServerCmdType::HostDynamic)
		return true;
	if (type == InterServerCmdType::RelayDynamic)
		return true;
	if (type == InterServerCmdType::Dynamic)
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ListenerHelperCommon::IsInterServerCommand(JNetwork::ICommand* _pCommand)
{
	const CmdType_t type = _pCommand->GetType();
	return type >= InterServerCmdType::InterServerBegin && type <= InterServerCmdType::InterServerEnd;
}
