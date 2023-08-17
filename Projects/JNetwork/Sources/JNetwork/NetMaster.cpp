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

NetMaster::~NetMaster() {
	// m_hNetGroup.Clear();
	// Finalize();
}

void NetMaster::Finalize() {
	if (m_bFinalized)
		return;

	m_hNetGroup.Clear();
	m_bFinalized = true;
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
