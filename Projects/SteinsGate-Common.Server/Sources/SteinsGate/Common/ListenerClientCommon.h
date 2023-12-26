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
	ListenerClientCommon(JNetwork::CommandParser* parser);

	void OnReceived(JNetwork::Session* session, JNetwork::ICommand* recvCmd) override;
	void OnReceived(JNetwork::Session* session, JNetwork::RecvedCommandPacket* recvPacket) override;
public:
	JNetwork::CommandParser* Parser;
};
