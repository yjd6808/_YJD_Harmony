/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 3:57:07 PM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/NetMaster.h>

NS_JNET_BEGIN

NetMaster::NetMaster()
	: m_hNetGroup(8)
	, m_bFinalized(false)
{}

NetMaster::~NetMaster() = default;

void NetMaster::Finalize() {
	if (m_bFinalized) {
		DebugAssert(false);
		return;
	}

	m_hNetGroup.ForEachValue([](NetGroupPtr& group) { group->Finalize(); });
	m_hNetGroup.Clear();
	m_bFinalized = true;
	_LogInfo_("%s %s 정리완료", m_szName.Source(), TypeName());
}

void NetMaster::SetName(const JCore::String& name) {
	m_szName = name;
}

void NetMaster::AddNetGroup(int groupId, const NetGroupPtr& group) {
	if (!m_hNetGroup.Insert(groupId, group)) {
		_NetLogWarn_("이미 해당 네트 그룹이 있습니다.");
	}
}

NetGroupPtr NetMaster::GetNetGroup(int id) {
	NetGroupPtr* pFind = m_hNetGroup.Find(id);

	if (pFind == nullptr) {
		_NetLogWarn_("%d ID의 네트 그룹이 없습니다.", id);
		return nullptr;
	}

	return *pFind;
}

NS_JNET_END
