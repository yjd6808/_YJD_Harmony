/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */

#pragma once

#include "sgcl/_Net/NetClientListenerImpl.h"

class NetClientGroup final : public jnet::NetGroup
{
public:
	NetClientGroup();
	~NetClientGroup() override;

	void Initialize() override;
	void Finalize() override;

	jnet::TcpClient* pAuthTcp_;
	jnet::UdpClient* pAuthUdp_;
	jnet::TcpClient* pLobbyTcp_;
	jnet::UdpClient* pLobbyUdp_;
	jnet::TcpClient* pLogicTcp_;
	jnet::UdpClient* pLogicUdp_;
	jnet::TcpClient* pAreaTcp_;
	jnet::UdpClient* pAreaUdp_;
	jnet::TcpClient* pChatTcp_;
	jnet::UdpClient* pChatUdp_;

	jnet::CommandParser& Parser() { return *pParser_; }
private:
	jnet::CommandParser* pParser_;

	friend class NetCore;
};
