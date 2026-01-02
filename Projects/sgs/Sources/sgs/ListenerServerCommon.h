/*
 * 작성자: 윤정도
 * 생성일: 5/12/2023 9:35:47 AM
 * =====================
 *
 */


#pragma once

#include <jnet/Packet/CommandParser.h>

#include <sg/ListenerServerBase.h>

class JC_NOVTABLE ListenerServerCommon : public ListenerServerBase
{
public:
	ListenerServerCommon(jnet::CommandParser* _pParser);

protected:
	void OnReceived(jnet::Session* _pSession, jnet::ICommand* _pRecvCmd) override;
	void OnReceived(jnet::Session* _pSession, jnet::RecvedCommandPacket* _pRecvPacket) override;

	virtual ServerType_t GetServerType() = 0;
	const char* GetServerName() override { return ServerType::Name[GetServerType()]; }

public:
	jnet::CommandParser* pParser_;
};
