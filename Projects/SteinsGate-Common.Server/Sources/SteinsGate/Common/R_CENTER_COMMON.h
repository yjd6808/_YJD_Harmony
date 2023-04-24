/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 6:42:25 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>
#include <SteinsGate/Common/CenterCmd.h>

struct R_CENTER_COMMON
{
public:
	static void RecvWhoAreYou(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RecvAlreadyConnected(JNetwork::Session* session, JNetwork::ICommand* cmd);
	static void RecvYouNeedToDoThis(JNetwork::Session* session, JNetwork::ICommand* cmd);
};