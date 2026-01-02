/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 4:04:21 PM
 * =====================
 * TCP 서버 및 넷 마스터 그루핑 테스트
 */


#pragma once

#include <jnet/Host/TcpServer.h>
#include <jnet/Host/UdpClient.h>
#include <jnet/NetGroupMgr.h>

#include <jnetr_common/ServerListener.h>
#include <jnetr_common/ClientListener.h>

NS_JNET_BEGIN

#define SERVER_LOGIN_CHANNEL_GROUP		1
#define SERVER_GAME_GROUP				2
#define SERVER_UDP_GROUP				3

//////////////////////////////////////////////////////////////////////////////////////////
class ServerNetGroupMgr : public NetGroupMgr
{
public:
	void Initialize() override;
};

//////////////////////////////////////////////////////////////////////////////////////////
class ServerLoginChannelNetGroup : public NetGroup
{
public:
	ServerLoginChannelNetGroup();
	~ServerLoginChannelNetGroup() override;

	void Initialize() override;

	TcpServer* GetLogin()
	{
		return loginServer_.GetPtr();
	}

	TcpServer* GetChannel()
	{
		return channelServer_.GetPtr();
	}

private:
	TcpServerWPtr loginServer_;
	TcpServerWPtr channelServer_;
};

//////////////////////////////////////////////////////////////////////////////////////////
class ServerGameNetGroup : public NetGroup
{
public:
	ServerGameNetGroup();
	~ServerGameNetGroup() override;

	void Initialize() override;

	TcpServer* GetGame()
	{
		return gameServer_.GetPtr();
	}

private:
	TcpServerWPtr gameServer_;
};

//////////////////////////////////////////////////////////////////////////////////////////
class ServerUdpNetGroup : public NetGroup
{
public:
	ServerUdpNetGroup();
	~ServerUdpNetGroup() override;

	void Initialize() override;

	UdpClient* GetLogin()
	{
		return loginUdpClient_.GetPtr();
	}

	UdpClient* GetChannel()
	{
		return channelUdpClient_.GetPtr();
	}

	UdpClient* GetGame()
	{
		return gameUdpClient_.GetPtr();
	}

private:
	UdpClientWPtr loginUdpClient_;
	UdpClientWPtr channelUdpClient_;
	UdpClientWPtr gameUdpClient_;
};


NS_JNET_END
