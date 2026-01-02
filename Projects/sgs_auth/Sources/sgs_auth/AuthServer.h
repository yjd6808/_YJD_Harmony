/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:53:19 PM
 * =====================
 *
 */


#pragma once


#include <sgs/CommonServer.h>

class AuthServer final : public CommonServer
{
public:
	AuthServer(const jnet::IOCPPtr& _pIocp, const jc::MemoryPoolAbstractPtr& _pBufferAllocator);

	jnet::TcpSession* CreateSession() override;
	ServerType_t GetServerType() override { return ServerType::Auth; }
protected:
	void OnUpdate(const jc::TimeSpan& _elapsed) override;
};
