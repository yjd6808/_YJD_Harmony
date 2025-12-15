/*
 * 작성자: 윤정도
 * 생성일: 5/12/2023 9:35:47 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet/CommandParser.h>

#include <SteinsGate/Common/ListenerServerBase.h>
#include <SteinsGate/Common/Enum.h>

class JCORE_NOVTABLE ListenerServerCommon : public ListenerServerBase
{
public:
	ListenerServerCommon(JNetwork::CommandParser* _pParser);

protected:
	void OnReceived(JNetwork::Session* _pSession, JNetwork::ICommand* _pRecvCmd) override;
	void OnReceived(JNetwork::Session* _pSession, JNetwork::RecvedCommandPacket* _pRecvPacket) override;

	virtual ServerType_t GetServerType() = 0;
	const char* GetServerName() override { return ServerType::Name[GetServerType()]; }

public:
	JNetwork::CommandParser* pParser_;
};
