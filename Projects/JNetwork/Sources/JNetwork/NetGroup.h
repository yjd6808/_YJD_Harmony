/*
 * 작성자: 윤정도
 * 생성일: 2/10/2023 11:41:08 AM
 * =====================
 * 삭제 순서 신경쓰기 싫어서 묶음
 */


#pragma once

#include <JCore/Pool/MemoryPoolAbstract.h>
#include <JNetwork/Host/Host.h>
#include <JNetwork/IOCP/IOCP.h>

NS_JNET_BEGIN

class JCORE_NOVTABLE NetGroup
{
public:
	NetGroup();
	NetGroup(const JCore::String& name);
	virtual ~NetGroup();

	void CreateIocp(int threadCount);
	void CreateBufferPool(const JCore::HashMap<int, int>& poolInfo);
	void RunIocp();
	void AddHost(const HostPtr& host);
	void SetName(const JCore::String& name);

	virtual void Initialize() = 0;
	virtual void Finalize();
protected:
	IOCPPtr m_spIOCP;
	JCore::Vector<HostPtr> m_vHostList;
	JCore::MemoryPoolAbstractPtr m_spBufferPool;
	JCore::String m_Name;
	bool m_bFinalized;
};

using NetGroupPtr = JCore::SharedPtr<NetGroup>;

NS_JNET_END