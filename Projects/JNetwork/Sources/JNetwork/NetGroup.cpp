/*
 * 작성자: 윤정도
 * 생성일: 2/10/2023 11:41:19 AM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/NetGroup.h>

#include <JCore/Pool/IndexedMemoryPool.h>


USING_NS_JC;

NS_JNET_BEGIN

NetGroup::NetGroup()
	: m_szName(0)
	, m_bFinalized(false)
{}

NetGroup::NetGroup(const String& name)
	: m_szName(name)
	, m_bFinalized(false)
{}

NetGroup::~NetGroup() {
	NetGroup::Finalize();
}

void NetGroup::CreateIocp(int threadCount) {
	m_spIOCP = MakeShared<IOCP>(threadCount);
	m_spIOCP->SetName(m_szName);
}

void NetGroup::CreateBufferPool(const HashMap<int, int>& poolInfo) {
	m_spBufferPool = MakeShared<IndexedMemoryPool>(poolInfo);
}

void NetGroup::RunIocp() {
	m_spIOCP->Run();
}

IOCPPtr NetGroup::GetIocp() {
	return m_spIOCP;
}

bool NetGroup::AddHost(int id, const HostPtr& host) {
	DebugAssertMsg(host != nullptr, "따끈따끈한 호스트만 넣어주세요");

	if (m_hHostMap.Exist(id)) {
		DebugAssert("%d 호스트가 이미 %s 넷그룹에 존재합니다.", id, m_Name.Source());
		return false;
	}

	m_hHostMap.Insert(id, host);
	return true;
}

void NetGroup::SetName(const String& name) {
	m_szName = name;
}

void NetGroup::Finalize() {

	if (m_bFinalized)
		return;

	m_hHostMap.ForEachValue([](const HostPtr& host) {
		DebugAssertMsg(host.RefCount() == 1, "넷 그룹 소멸전에 외부 레퍼런스를 모두 정리해주세요. (윅포를 사용해주세요)");
	});

	m_hHostMap.Clear();
	_LogInfo_("%s %s 호스트 정리완료", m_szName.Source(), TypeName());

	if (m_spIOCP.Exist()) {
		m_spIOCP->Join();
		_LogInfo_("%s %s IOCP 쪼인완료", m_szName.Source(), TypeName());
		m_spIOCP->Destroy();
		_LogInfo_("%s %s IOCP 파괴완료", m_szName.Source(), TypeName());
		m_spIOCP = nullptr;
	}

	m_spBufferPool = nullptr;
	_LogInfo_("%s %s 버퍼풀 정리완료", m_szName.Source(), TypeName());
	m_bFinalized = true;
}


NS_JNET_END