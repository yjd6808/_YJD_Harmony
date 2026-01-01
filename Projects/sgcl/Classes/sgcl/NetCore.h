/*
 * 작성자: 윤정도
 * 생성일: 3/29/2023 8:46:53 AM
 * =====================
 *
 */

#pragma once

#include <sgcl/NetClientGroup.h>
#include <sgcl/CommandSynchronizer.h>
#include <sgcl/ConnectionSynchronizer.h>
#include <sgcl/ComponentCollection.h>

class AuthenticationComponent;

class NetCore
	: public jnet::NetGroupMgr
	, public jc::SingletonPointer<NetCore>
{
	friend class TSingleton;
	NetCore();
	~NetCore() override;

public:
	void Initialize() override;
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

	CommandSynchronizer* GetCommandSynchronizer() const { return pCommandSynchronizer_; }
	ConnectionSynchronizer* GetConnectionSynchronizer() const { return pConnectionSynchronizer_; }

	void InitializeComponents();

	AuthenticationComponent* GetAuthenticationComponent() const { return pAuthentication_; }

	bool ConnectAuthTcp();
	bool ConnectLobbyTcp();

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
	ComponentCollection componentCollection_;

	AuthenticationComponent* pAuthentication_;
};
