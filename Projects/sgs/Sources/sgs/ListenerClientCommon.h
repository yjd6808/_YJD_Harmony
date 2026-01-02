/*
 * 작성자: 윤정도
 * 생성일: 5/12/2023 9:36:48 AM
 * =====================
 *
 */


#pragma once


#include <sgs/ListenerHelperCommon.h>
#include <sg/ListenerClientBase.h>
#include <jnet/Packet/CommandParser.h>

class JC_NOVTABLE ListenerClientCommon : public ListenerClientBase
{
protected:
	ListenerClientCommon(jnet::CommandParser* _pParser);

	void OnReceived(jnet::Session* _pSession, jnet::ICommand* _pRecvCmd) override;
	void OnReceived(jnet::Session* _pSession, jnet::RecvedCommandPacket* _pRecvPacket) override;

public:
	jnet::CommandParser* pParser_;
};
