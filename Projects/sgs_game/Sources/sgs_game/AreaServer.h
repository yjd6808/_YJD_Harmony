/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once


#include <sgs/CommonServer.h>

class AreaServer final : public CommonServer
{
public:
	AreaServer(const jnet::IOCPPtr& _pIocp, const jc::MemoryPoolAbstractPtr& _pBufferAllocator);
	~AreaServer() noexcept override;

	jnet::TcpSession* CreateSession() override;
	ServerType_t GetServerType() override
	{
		return ServerType::Area;
	}
protected:
	void OnUpdate(const jc::TimeSpan& _elapsed) override;
};
