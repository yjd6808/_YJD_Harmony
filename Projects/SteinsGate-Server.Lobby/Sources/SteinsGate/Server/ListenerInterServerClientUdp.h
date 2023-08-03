﻿/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
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