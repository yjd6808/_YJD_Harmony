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
	explicit NetGroup(const JCore::String& _name);
	virtual ~NetGroup();

	void CreateIocp(int _threadCount);
	void CreateBufferPool(const JCore::HashMap<int, int>& _poolInfo);
	void RunIocp();
	IOCPPtr GetIocp();
	bool AddHost(int _id, const HostPtr& _pHost);
	void SetName(const JCore::String& _name);

	template <typename T>
	T* GetHost(int _id)
	{
		static_assert(JCore::IsNaturalType_v<T>, "... T must be natural type");
		const HostPtr* pFind = hostMap_.Find(_id);

		if (pFind == nullptr)
		{
			_LogWarn_("%s %s에서 %d 호스트 검색에 실패했습니다.", name_.Source(), TypeName(), _id);
			return nullptr;
		}

		T* pHost = dynamic_cast<T*>(pFind->GetPtr());
		if (pHost == nullptr)
		{
			_LogWarn_("%s %s에서 %d 호스트는 %s 타입이 아닙니다.", name_.Source(), TypeName(), _id, typeid(T).name());
			return nullptr;
		}

		return pHost;
	}

	virtual void Initialize() = 0;
	virtual void Finalize();

	static constexpr const char* TypeName() { return "네트그룹"; }

protected:
	IOCPPtr pIocp_;
	JCore::HashMap<int, HostPtr> hostMap_;
	JCore::MemoryPoolAbstractPtr pBufferPool_;
	JCore::String name_;
	bool finalized_;
};

using NetGroupPtr = JCore::SharedPtr<NetGroup>;

NS_JNET_END
