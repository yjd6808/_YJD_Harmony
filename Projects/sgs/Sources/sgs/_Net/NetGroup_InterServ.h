/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 2:24:28 AM
 * =====================
 * 모든 서버 프로세스의 공통 인터서버 클라이언트 그룹
 * 단, 중앙서버는 릴레이 주체이므로 TcpClient는 사용하지 않는다.
 */


#pragma once

#include "jnet/NetGroup.h"
#include "sgs/IUpdatable.h"

NS_SG_BEGIN

class NetGroup_InterServ : public jnet::NetGroup, public IUpdatable, public jc::SingletonSPointer<NetGroup_InterServ>
{
public:
	NetGroup_InterServ();
	~NetGroup_InterServ() override;

	void Initialize() override;
	void Finalize() override;

	bool ConnectCenterServer(int _tryCount);
	virtual void OnUpdate(const jc::TimeSpan& _elapsed) override;

	jnet::CommandParser&	Parser() { return *pParser_; }
	jnet::TcpClient*		GetTcp() const { return pTcp_; }
	jnet::UdpClient*		GetUdp() const { return pUdp_; }

protected:
	void SyncPeerServerTime(const jc::TimeSpan& _elapsed);

	jnet::TcpClient* pTcp_;
	jnet::UdpClient* pUdp_;
	jnet::CommandParser* pParser_;
};

NS_END

#define g_cNetGroup_InterServ JC_DECL_SINGLETON_BODY(sg::NetGroup_InterServ)
