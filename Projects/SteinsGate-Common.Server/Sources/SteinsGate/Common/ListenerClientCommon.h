/*
 * 작성자: 윤정도
 * 생성일: 5/12/2023 9:36:48 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/ListenerHelperCommon.h>
#include <SteinsGate/Common/ListenerClientBase.h>
#include <JNetwork/Packet/CommandParser.h>

class JCORE_NOVTABLE ListenerClientCommon : public ListenerClientBase
{
protected:
	ListenerClientCommon(JNetwork::CommandParser* _pParser);

	void OnReceived(JNetwork::Session* _pSession, JNetwork::ICommand* _pRecvCmd) override;
	void OnReceived(JNetwork::Session* _pSession, JNetwork::RecvedCommandPacket* _pRecvPacket) override;

public:
	JNetwork::CommandParser* pParser_;
};
