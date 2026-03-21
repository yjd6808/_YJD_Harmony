/*
 * 작성자: 윤정도
 * 생성일: 3/29/2023 8:46:53 AM
 * =====================
 *
 */

#pragma once

#include "sgcl/NetClientGroup.h"

class CommandSynchronizer;
class ConnectionSynchronizer;
class AuthenticationComponent;

class NetCore : public jnet::NetGroupMgr, public jc::SingletonPointer<NetCore>
{
	friend class TSingleton;
	NetCore();
	~NetCore() override;

public:
	bool Initialize() override;
	void Finalize() override;
	void PollNetEvents();
	void RunCommand(jnet::Session* _pSession, jnet::ICommand* _pCmd);

	NetClientGroup* GetGroup() const { return pNetClientGroup_; }
	jnet::TcpClient* GetAuthTcp() const { return pAuthTcp_; }
	jnet::TcpClient* GetLobbyTcp() const { return pLobbyTcp_; }
	jnet::TcpClient* GetLogicTcp() const { return pLogicTcp_; }
	jnet::UdpClient* GetLogicUdp() const { return pLogicUdp_; }
	jnet::TcpClient* GetChatTcp() const { return pChatTcp_; }
	jnet::TcpClient* GetAreaTcp() const { return pAreaTcp_; }

	CommandSynchronizer* GetCommandSynchronizer() { return pCommandSynchronizer_; }
	ConnectionSynchronizer* GetConnectionSynchronizer() { return pConnectionSynchronizer_; }

	const jnet::IPv4EndPoint& GetAuthServerEndPoint() const { return authEp_; }

	bool ConnectAuthTcp();
	bool ConnectLobbyTcp();

public:
	AuthenticationComponent&	authentication_;

	jnet::IPv4EndPoint authEp_;

private:
	NetClientGroup* pNetClientGroup_;
	jnet::TcpClient* pAuthTcp_;
	jnet::TcpClient* pLobbyTcp_;
	jnet::TcpClient* pLogicTcp_;
	jnet::UdpClient* pLogicUdp_;
	jnet::TcpClient* pChatTcp_;
	jnet::TcpClient* pAreaTcp_;

	CommandSynchronizer* pCommandSynchronizer_;
	ConnectionSynchronizer* pConnectionSynchronizer_;
};

#define g_cNet JC_DECL_SINGLETON_BODY(NetCore)