/*
 * 작성자: 윤정도
 * 생성일: 2/10/2023 11:41:19 AM
 * =====================
 *
 */


#include <jnet/Core.h>
#include <jnet/NetGroup.h>

#include <jc/Pool/IndexedMemoryPool.h>


USING_NS_JC;

NS_JNET_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
NetGroup::NetGroup()
: name_(0)
, finalized_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
NetGroup::NetGroup(const String& _name)
: name_(_name)
, finalized_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
NetGroup::~NetGroup() = default;

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup::CreateIocp(int _threadCount)
{
	pIocp_ = MakeShared<IOCP>(_threadCount);
	pIocp_->SetName(name_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup::CreateBufferPool(const HashMap<int, int>& _poolInfo)
{
	pBufferPool_ = MakeShared<CIndexedMemoryPool>(_poolInfo);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup::RunIocp()
{
	pIocp_->Run();
}

//////////////////////////////////////////////////////////////////////////////////////////
IOCPPtr NetGroup::GetIocp()
{
	return pIocp_;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NetGroup::AddHost(int _id, const HostPtr& _pHost)
{
	jc_assert_msg(_pHost != nullptr, "따끈따끈한 호스트만 넣어주세요");

	if (hostMap_.Exist(_id))
	{
		_NetLogWarn_("%d 호스트가 이미 %s 넷그룹에 존재합니다.", _id, name_.Source());
		return false;
	}

	hostMap_.Insert(_id, _pHost);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup::SetName(const String& _name)
{
	name_ = _name;
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup::Finalize()
{
	if (finalized_)
	{
		jc_assert(false);
		return;
	}

	hostMap_.ForEachValue([](const HostPtr& _pHost)
	{
		jc_assert_msg(_pHost.RefCount() == 1, "넷 그룹 소멸전에 외부 레퍼런스를 모두 정리해주세요. (윅포를 사용해주세요)");
	});

	hostMap_.Clear();
	_LogInfo_("%s %s 호스트 정리완료", name_.Source(), TypeName());

	if (pIocp_.Exist())
	{
		pIocp_->Join();
		_LogInfo_("%s %s IOCP 쪼인완료", name_.Source(), TypeName());
		pIocp_->Destroy();
		_LogInfo_("%s %s IOCP 파괴완료", name_.Source(), TypeName());
		pIocp_ = nullptr;
	}

	pBufferPool_ = nullptr;
	_LogInfo_("%s %s 버퍼풀 정리완료", name_.Source(), TypeName());
	finalized_ = true;
}

NS_JNET_END
