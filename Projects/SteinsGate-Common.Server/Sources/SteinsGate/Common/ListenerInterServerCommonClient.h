/*
 * 작성자: 윤정도
 * 생성일: 6/17/2023 4:01:52 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/ListenerCommonClient.h>

class ListenerInterServerCommonClient : public ListenerCommonClient
{
protected:
	void OnReceivedInterServerCmd(JNetwork::Session* sender, JNetwork::ICommand* cmd);
};
