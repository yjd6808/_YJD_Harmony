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
	IOCPPtr GetIocp();
	bool AddHost(int id, const HostPtr& host);
	void SetName(const JCore::String& name);

	template <typename T>
	T* GetHost(int id) {
		static_assert(JCore::IsNaturalType_v<T>, "... T must be natural type");
		const HostPtr* pFind = m_hHostMap.Find(id);

		if (pFind == nullptr) {
			_LogWarn_("%s %s에서 %d 호스트 검색에 실패했습니다.", m_szName.Source(), TypeName(), id);
			return nullptr;
		}

		T* pHost = dynamic_cast<T*>(pFind->GetPtr());
		if (pHost == nullptr) {
			_LogWarn_("%s %s에서 %d 호스트는 %s 타입이 아닙니다.", m_szName.Source(), TypeName(), id, typeid(T).name());
			return nullptr;
		}

		return pHost;
	}

	virtual void Initialize() = 0;
	virtual void Finalize();

	static constexpr const char* TypeName() { return "네트그룹"; }
protected:
	IOCPPtr m_spIOCP;
	JCore::HashMap<int, HostPtr> m_hHostMap;
	JCore::MemoryPoolAbstractPtr m_spBufferPool;
	JCore::String m_szName;
	bool m_bFinalized;
};

using NetGroupPtr = JCore::SharedPtr<NetGroup>;

NS_JNET_END