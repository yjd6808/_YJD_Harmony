/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:19:42 PM
 * =====================
 *
 */


#include "Center.h"
#include "CenterCoreHeader.h"
#include "CenterServer.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
CenterServer::CenterServer(const IOCPPtr& _pIocp, const MemoryPoolAbstractPtr& _pBufferAllocator)
	: CommonServer(_pIocp, _pBufferAllocator)
	, startupLaunching_(true)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
CenterServer::~CenterServer()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SGTcpSession* CenterServer::CreateSession()
{
	return dbg_new CenterSession(this, pIocp_, pBufferAllocator_, 4096, 4096);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterServer::AddSession(CenterSession* _pSession)
{
	DebugAssert(_pSession->IsValid());
	const Int8 serverId = _pSession->GetServerId();

	centerSessions_[serverId].Session = _pSession;
	centerSessions_[serverId].Type = _pSession->GetClientType();
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterServer::RemoveSession(CenterSession* _pSession)
{
	const Int8 serverId = _pSession->GetServerId();

	centerSessions_[serverId].Session = nullptr;
	centerSessions_[serverId].Type = ServerProcessType::None;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterServer::BroadcastPacket(IPacket* _pPacket)
{
	const Vector<int>& activeServerIdList = Core::ServerProcessInfoPackage->activeServerIdList_;
	const int serverCount = activeServerIdList.Size();

	for (int serverIndex = 0; serverIndex < serverCount; ++serverIndex)
	{
		Session* pSession = centerSessions_[activeServerIdList[serverIndex]].Session;
		if (pSession == nullptr)
			continue;

		pSession->SendAsync(_pPacket);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CenterServer::IsAllCenterSessionConnected()
{
	const Vector<int>& activeServerIdList = Core::ServerProcessInfoPackage->activeServerIdList_;
	const int serverCount = activeServerIdList.Size();

	for (int serverIndex = 0; serverIndex < serverCount; ++serverIndex)
	{
		if (centerSessions_[activeServerIdList[serverIndex]].Session == nullptr)
			return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CenterServer::IsConnected(CenterSession* _pSession)
{
	for (int serverId = 0; serverId < Const::Server::MaxProcessId; ++serverId)
	{
		if (centerSessions_[serverId].Session == _pSession)
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CenterServer::IsConnected(int _serverId)
{
	if (centerSessions_[_serverId].Session != nullptr)
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterServer::OnUpdate(const TimeSpan& _elapsed)
{
	CommonServer::OnUpdate(_elapsed);
}

