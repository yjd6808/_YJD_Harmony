/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/NetClientEventListener.h>

class NetClientGroup final : public SGNetGroup
{
public:
	NetClientGroup();
	~NetClientGroup() override;

	void Initialize() override;
	void Finalize() override;

	SGTcpClient* pAuthTcp_;
	SGUdpClient* pAuthUdp_;
	SGTcpClient* pLobbyTcp_;
	SGUdpClient* pLobbyUdp_;
	SGTcpClient* pLogicTcp_;
	SGUdpClient* pLogicUdp_;
	SGTcpClient* pAreaTcp_;
	SGUdpClient* pAreaUdp_;
	SGTcpClient* pChatTcp_;
	SGUdpClient* pChatUdp_;

	SGCommandParser* GetParser() { return &parser_; }

private:
	SGCommandParser parser_;

	friend class NetCore;
};
