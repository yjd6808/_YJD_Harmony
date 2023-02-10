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
{}

void NetMaster::Finalize() {
	m_hNetGroup.Clear();
}

void NetMaster::AddNetGroup(int groupId, const NetGroupPtr& group) {
	DebugAssertMsg(m_hNetGroup.Exist(groupId) == false, "이미 해당 네트 그룹이 있습니다.");
	m_hNetGroup.Insert(groupId, group);
}

NS_JNET_END
