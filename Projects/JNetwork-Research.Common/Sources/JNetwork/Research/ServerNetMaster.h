/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:04:21 PM
 * =====================
 * TCP 서버 및 넷 마스터 그루핑 테스트
 */


#pragma once

#include <JNetwork/Host/TcpServer.h>
#include <JNetwork/Host/UdpClient.h>
#include <JNetwork/NetMaster.h>

#include <JNetwork/Research/ServerListener.h>
#include <JNetwork/Research/ClientListener.h>



NS_JNET_BEGIN

#define ServerLoginChannelGroup		1
#define ServerGameGroup				2
#define ServerUdpGroup				3

class ServerNetMaster : public NetMaster
{
public:
	void Initialize() override;
};

class ServerLoginChannelNetGroup : public NetGroup
{
public:
	ServerLoginChannelNetGroup();
	~ServerLoginChannelNetGroup() override;

	void Initialize() override;

	TcpServer* GetLogin() { return m_spLoginServer.GetPtr(); }
	TcpServer* GetChannel() { return m_spChannelServer.GetPtr(); }
private:
	TcpServerWPtr m_spLoginServer;
	TcpServerWPtr m_spChannelServer;

	ServerListener m_LoginListener;
	ServerListener m_ChannelListener;
};

class ServerGameNetGroup : public NetGroup
{
public:
	ServerGameNetGroup();
	~ServerGameNetGroup() override;

	void Initialize() override;

	TcpServer* GetGame() { return m_spGameServer.GetPtr(); }
private:
	TcpServerWPtr m_spGameServer;
	ServerListener m_GameListener;
};

class ServerUdpNetGroup : public NetGroup
{
public:
	ServerUdpNetGroup();
	~ServerUdpNetGroup() override;

	void Initialize() override;

	UdpClient* GetLogin() { return m_spLoginUdpClient.GetPtr(); }
	UdpClient* GetChannel() { return m_spChannelUdpClient.GetPtr(); }
	UdpClient* GetGame() { return m_spGameUdpClient.GetPtr(); }

private:
	UdpClientWPtr m_spLoginUdpClient;
	UdpClientWPtr m_spChannelUdpClient;
	UdpClientWPtr m_spGameUdpClient;

	ClientListener m_LoginListener;
	ClientListener m_ChannelListener;
	ClientListener m_GameListener;
};



NS_JNET_END