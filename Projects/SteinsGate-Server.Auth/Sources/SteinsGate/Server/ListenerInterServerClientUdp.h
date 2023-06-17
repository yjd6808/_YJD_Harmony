/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 8:44:27 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ListenerCommonClient.h>

class ListenerInterServerClientUdp : public ListenerCommonClient
{
public:
	ListenerInterServerClientUdp();
protected:
	void OnReceived(SGSession* ssession, JNetwork::ICommand* cmd) override;
};