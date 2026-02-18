/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 7:01:50 AM
 * =====================
 *
 */

#include "Core.h"
#include "DescMgr_ServerInfo.h"

#include <sg/_API/sgapi_Util.h>

#include <sg/_Util/JsonUtil.h>

USING_NS_JS;
USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
ServerInfoLoader::ServerInfoLoader()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ServerInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
	{
		return false;
	}

	try
	{
		Json::Value& serverInfoRootList = root["server"];

		ServerProcessType_t processType = (ServerProcessType_t)sgapi_GetServerProcessType();
		if (processType < ServerProcessType::Begin && processType > ServerProcessType::End)
			throw std::runtime_error("잘못된 프로세스 타입입니다.");

		for (ArrayIndex serverInfoIndex = 0; serverInfoIndex < serverInfoRootList.size(); ++serverInfoIndex)
		{
			ServerProcessInfo* pServerInfo = nullptr;
			Value& serverInfoRoot = serverInfoRootList[serverInfoIndex];
			switch (processType)
			{
			case ServerProcessType::Auth:
				{
					AuthServerProcessInfo* pServerInfoDetail = dbg_new AuthServerProcessInfo();
					pServerInfo = pServerInfoDetail;
					ReadInterServInfo(serverInfoRoot, pServerInfoDetail->interServerInfo_);
				}
			case ServerProcessType::Lobby:
				{
					LobbyServerProcessInfo* pServerInfoDetail = dbg_new LobbyServerProcessInfo();
					pServerInfo = pServerInfoDetail;
					ReadInterServInfo(serverInfoRoot, pServerInfoDetail->interServerInfo_);
				}
			case ServerProcessType::Game:
				{
					GameServerProcessInfo* pServerInfoDetail = dbg_new GameServerProcessInfo();
					pServerInfo = pServerInfoDetail;
					ReadInterServInfo(serverInfoRoot, pServerInfoDetail->interServerInfo_);
				}
				break;
			case ServerProcessType::Center:
				{
					CenterServerProcessInfo* pServerInfoDetail = dbg_new CenterServerProcessInfo();
					pServerInfo = pServerInfoDetail;
				}
				break;
			}
			jc_assert(pServerInfo != nullptr);
			ReadCommonInfo(serverInfoRoot, pServerInfo);
			AddData(pServerInfo);
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	Loaded();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerInfoLoader::ReadCommonInfo(Json::Value& _serverRoot, OUT ServerProcessInfo* _pServerInfo)
{
	_pServerInfo->code_ = _serverRoot["code"].asInt();
	_pServerInfo->processName_ = JsonUtil::GetStringOrDefault(_serverRoot["name"]);
	_pServerInfo->mainIOCPThreadCount_ = _serverRoot["main_iocp_thread_count"].asInt();
	_pServerInfo->updatePerSecond_ = _serverRoot["update_per_second"].asInt();

	Json::Value& netServerInfoList = _serverRoot["net_server_info"];
	for (ArrayIndex i = 0; i < netServerInfoList.size(); ++i)
	{
		Value& netServerInfo = netServerInfoList[i];

		NetServerInfo info;
		jc::String bindTcp = JsonUtil::GetString(_serverRoot["bind_tcp"]);
		jc::String bindUdp = JsonUtil::GetString(_serverRoot["bind_udp"]);
		jc::String remote = JsonUtil::GetString(_serverRoot["remote_ep"]);

		info.serverType_ = _serverRoot["server_type"].asInt();
		info.serverName_ = JsonUtil::GetStringOrDefault(netServerInfo["server_name"]);
		info.bindTcp_ = IPv4EndPoint{ bindTcp };
		info.bindUdp_ = IPv4EndPoint{ bindUdp };
		info.remoteEp_ = IPv4EndPoint{ remote };
		info.tcpRecvBufferSize_ = netServerInfo["tcp_recv_buffer_size"].asInt();
		info.tcpSendBufferSize_ = netServerInfo["tcp_send_buffer_size"].asInt();
		info.udpRecvBufferSize_ = netServerInfo["udp_recv_buffer_size"].asInt();
		info.udpSendBufferSize_ = netServerInfo["udp_send_buffer_size"].asInt();
		info.maxSessionCount_ = netServerInfo["max_session_count"].asInt();
		_pServerInfo->mainServerInfoList_.PushBack(info);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ServerInfoLoader::ReadInterServInfo(Json::Value& _serverRoot, OUT NetInterServerInfo& _interServerInfo)
{
	jc::String bindInterServerUdp = JsonUtil::GetString(_serverRoot["bind_interserver_udp"]);
	jc::String bindInterServerTcp = JsonUtil::GetString(_serverRoot["bind_interserver_tcp"]);
	jc::String remoteCenterServerEp = JsonUtil::GetString(_serverRoot["remote_center_server_ep"]);

	_interServerInfo.bindUdp_ = IPv4EndPoint{ bindInterServerUdp };
	_interServerInfo.bindTcp_ = IPv4EndPoint{ bindInterServerTcp };
	_interServerInfo.remoteCenterServerEp_ = IPv4EndPoint{ remoteCenterServerEp };
	_interServerInfo.tcpRecvBufferSize_ = _serverRoot["interserver_tcp_recv_buffer_size"].asInt();
	_interServerInfo.tcpSendBufferSize_ = _serverRoot["interserver_tcp_send_buffer_size"].asInt();
	_interServerInfo.udpRecvBufferSize_ = _serverRoot["interserver_udp_recv_buffer_size"].asInt();
	_interServerInfo.udpSendBufferSize_ = _serverRoot["interserver_udp_send_buffer_size"].asInt();
	_interServerInfo.iocpThreadCount_ = _serverRoot["interserver_iocp_thread_count"].asInt();
}

