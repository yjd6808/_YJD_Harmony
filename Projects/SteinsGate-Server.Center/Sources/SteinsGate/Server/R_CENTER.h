/*
 * 작성자: 윤정도
 * 생성일: 4/18/2023 2:19:13 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>
#include <SteinsGate/Common/CenterCmd.h>

struct R_CENTER
{
public:
	static void RecvItsMe(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RecvCenterMessage(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RecvNotifyBootState(JNetwork::Session* session, JNetwork::ICommand* cmd);
};