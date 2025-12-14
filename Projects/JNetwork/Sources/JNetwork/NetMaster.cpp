/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 3:57:07 PM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/NetMaster.h>

NS_JNET_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
NetMaster::NetMaster()
	: netGroupMap_(8)
	, finalized_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
NetMaster::~NetMaster() = default;

//////////////////////////////////////////////////////////////////////////////////////////
void NetMaster::Finalize()
{
	if (finalized_)
	{
		DebugAssert(false);
		return;
	}

	netGroupMap_.ForEachValue([](NetGroupPtr& _pGroupPtr)
	{
		_pGroupPtr->Finalize();
	});

	netGroupMap_.Clear();
	finalized_ = true;
	_LogInfo_("%s %s 정리완료", name_.Source(), TypeName());
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetMaster::SetName(const JCore::String& _name)
{
	name_ = _name;
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetMaster::AddNetGroup(int _groupId, const NetGroupPtr& _pGroupPtr)
{
	if (!netGroupMap_.Insert(_groupId, _pGroupPtr))
	{
		_NetLogWarn_("이미 해당 네트 그룹이 있습니다.");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
NetGroupPtr NetMaster::GetNetGroup(int _id)
{
	NetGroupPtr* pFind = netGroupMap_.Find(_id);

	if (pFind == nullptr)
	{
		_NetLogWarn_("%d ID의 네트 그룹이 없습니다.", _id);
		return nullptr;
	}

	return *pFind;
}

NS_JNET_END
