/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ListenerClientCommon.h>

class ListenerInterServerHostUdp : public ListenerClientCommon
{
public:
	ListenerInterServerHostUdp();
protected:
	void OnReceived(SGSession* ssession, JNetwork::ICommand* cmd) override;
};