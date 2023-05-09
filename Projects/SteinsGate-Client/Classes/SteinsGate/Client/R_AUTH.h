/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 1:57:31 PM
 * =====================
 *
 */


#pragma once



#include <JNetwork/Host/Session.h>
#include <SteinsGate/Common/NetAuthCmd.h>

struct R_AUTH
{
public:
	static void RecvLoginAck(JNetwork::Session* session, JNetwork::ICommand* cmd);
};
