/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 1:52:07 PM
 * =====================
 *
 */

#include "NetGroup_Main.h"

#include "sg/_Net/NetServerListener.h"

#include "sgs/_Net/NetCore.h"
#include "sgs/_Net/NetServer.h"

USING_NS_JC;
USING_NS_JNET;
USING_NS_SG;

//////////////////////////////////////////////////////////////////////////////////////////
NetGroup_Main::NetGroup_Main()
: pParser_(dbg_new CommandParser)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
NetGroup_Main::~NetGroup_Main()
{
	JC_DELETE_SAFE(pParser_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup_Main::Initialize()
{
	ServerProcessType_t processType = (ServerProcessType_t)g_cNetCore.GetProcessType();
	const ServerProcessInfo& processInfo = g_cNetCore.GetProcessInfo();

	int channelNumber = -1;
	int channelType_ = -1;
	int gameServerType = GameServerType::None;
	if (processType == ServerProcessType::Game)
	{
		const auto& gameServerProcessInfo = static_cast<const GameServerProcessInfo&>(processInfo);
		channelNumber = gameServerProcessInfo.channelNumber_;
		channelType_ = gameServerProcessInfo.channelType_;
		gameServerType = gameServerProcessInfo.gameServerType_;
	}

	CreateBufferPool({});
	CreateIOCP(processInfo.mainIOCPThreadCount_);
	RunIOCP();

	for (int i = 0; i < processInfo.mainServerInfoList_.Size() && i < MAX_SERVER_COUNT; ++i)
	{
		NetServerInfo& serverInfo = processInfo.mainServerInfoList_[i];
		auto pServer = MakeShared<NetServer>(pIocp_, pBufferPool_);
		int idx = serverInfo.serverType_;
		object_id serverHandle = make_net_server_handle(NET_GID_MAIN, idx);

		// 게임서버 프로세스이고, 게임서버인 경우
		if (processType == ServerProcessType::Game && idx == ServerType::Game)
		{
			serverHandle = make_net_game_server_handle(NET_GID_MAIN, gameServerType, channelNumber);
		}

		AddHost(idx, pServer);

		// 모든 서버상 세션의 ID를 고유하게 만듬.
		// 예시) 게임서버 프로세스의 서버 ID는 1001
		// 		그러면 이 게임서버 프로세스에서 구동하는 모든 서버의 세션 ID는 1001xxxxx가 된다.
		pSessionContainerArr_[idx] = dbg_new SessionContainer(serverInfo.maxSessionCount_); // 0번 인덱스는 안씀
		pSessionContainerArr_[idx]->SetInitialHandleSeq(serverHandle);

		pServerArr_[idx] = pServer.Get<NetServer*>();
		pServerArr_[idx]->SetHandle(serverHandle);
		pServerArr_[idx]->SetServerInfo(serverInfo);
		pServerArr_[idx]->SetSesssionContainer(pSessionContainerArr_[idx]);
		pServerArr_[idx]->SetEventListener(dbg_new NetServerListener { pServerArr_[idx], pParser_ });

		AddUpdatable(idx, pServerArr_[idx]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup_Main::Finalize()
{
	NetGroup::Finalize();
	JC_DELETE_SAFE(pParser_);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NetGroup_Main::AddUpdatable(int _id, IUpdatable* _pUpdatable)
{
	if (!updatableCollection_.Add(_id, _pUpdatable))
	{
		_LogWarn_("넷그룹 업데이터블 추가실패(%d)", _id);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup_Main::OnUpdate(const TimeSpan& _elapsed)
{
	updatableCollection_.Update(_elapsed);
}

//////////////////////////////////////////////////////////////////////////////////////////
NetSession* NetGroup_Main::GetSession(object_id _handle)
{
	if (get_group_id_from_sh(_handle) != NET_GID_MAIN)
	{
		jc_assert(false); // 이상한 거 전달함.
		return nullptr;
	}

	_u8 hostClass = get_host_class_from_sh(_handle);
	_u16 sessionIdx = get_session_idx_from_sh(_handle);
	if (sessionIdx == 0)
	{
		jc_assert(false); // 세션 인덱스는 1부터 시작해야함.
		return nullptr;
	}

	if (hostClass == nhGameServer)
	{
		return (NetSession*)pSessionContainerArr_[ServerType::Game]->Get(sessionIdx);
	}
	if (hostClass == nhServer)
	{
		return (NetSession*)pSessionContainerArr_[get_server_type_from_sh(_handle)]->Get(sessionIdx);
	}
	jc_assert(false); // 이상한 거 전달함.
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup_Main::ProcessOrder(CenterOrder_t _order, jc::String _arg)
{
	switch (_order)
	{
	case CenterOrder::LaunchServer:
		LaunchServer();
		break;
	case CenterOrder::StopServer:
		StopServer();
		break;
	default:
		jc_assert(false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup_Main::LaunchServer()
{
	JC_LOCK_GUARD(serverBootLock_);

	for (int i = 0; i < hostList_.Size(); ++i)
	{
		NetServer* pServer = hostList_[i].Get<NetServer*>();

		const ServerBootState_t state = pServer->GetBootState();
		if (state == ServerBootState::Launched || state == ServerBootState::Launching)
			continue;
		pServer->SetBootState(ServerBootState::Launching);
		pServer->Start();
	}
	
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup_Main::StopServer()
{
	JC_LOCK_GUARD(serverBootLock_);
	for (int i = 0; i < hostList_.Size(); ++i)
	{
		NetServer* pServer = hostList_[i].Get<NetServer*>();
		const ServerBootState_t state = pServer->GetBootState();

		if (state == ServerBootState::Stopped || state == ServerBootState::Stopping)
			return;
		pServer->SetBootState(ServerBootState::Stopping);
		pServer->Stop();
	}
}
