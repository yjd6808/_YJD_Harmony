/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 3:57:07 PM
 * =====================
 *
 */


#include <jnet/Core.h>
#include <jnet/NetGroupMgr.h>

NS_JNET_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
NetGroupMgr::NetGroupMgr()
	: netGroupMap_(8)
	, finalized_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
NetGroupMgr::~NetGroupMgr() = default;

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroupMgr::Finalize()
{
	if (finalized_)
		return;

	netGroupMap_.ForEachValue([](NetGroupPtr& _pGroupPtr)
	{
		_pGroupPtr->Finalize();
	});

	netGroupMap_.Clear();
	finalized_ = true;
	_LogInfo_("%s %s 정리완료", name_.Source(), TypeName());
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroupMgr::SetName(const jc::String& _name)
{
	name_ = _name;
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroupMgr::AddNetGroup(int _groupId, const NetGroupPtr& _pGroupPtr)
{
	if (validator_ != nullptr && !validator_(_pGroupPtr))
	{
		jc_assert(false);
		_NetLogWarn_("유효하지 않은 네트 그룹입니다.");
		return;
	}

	if (!netGroupMap_.Insert(_groupId, _pGroupPtr))
	{
		_NetLogWarn_("이미 해당 네트 그룹이 있습니다.");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
NetGroupPtr NetGroupMgr::GetNetGroup(int _id)
{
	NetGroupPtr* pFind = netGroupMap_.Find(_id);

	if (pFind == nullptr)
	{
		_NetLogWarn_("%d ID의 네트 그룹이 없습니다.", _id);
		return nullptr;
	}

	return *pFind;
}

NS_END
