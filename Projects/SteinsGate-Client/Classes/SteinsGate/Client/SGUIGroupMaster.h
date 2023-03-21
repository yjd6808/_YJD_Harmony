/*
 * 작성자: 윤정도
 * 생성일: 3/20/2023 9:26:21 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGUIGroup.h>

class SGUIGroupMaster : public cocos2d::Ref
{
public:
	SGUIGroupMaster(SGUIGroupInfo* info);
	~SGUIGroupMaster() override;

	static SGUIGroupMaster* createRetain();
	void init();
	void addMasterGroup(SGUIGroup* group);
	void forEach(const SGActionFn<SGUIGroup*>& action);
private:
	SGUIGroupInfo* m_pInfo;
	SGVector<SGUIGroup*> m_vMasterGroupList;
};

