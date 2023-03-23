/*
 * 작성자: 윤정도
 * 생성일: 3/20/2023 9:26:21 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGUIMasterGroup.h>


class SGUIGroupMaster : public cocos2d::Ref
{
public:
	SGUIGroupMaster(SGUIGroupInfo* info);
	~SGUIGroupMaster() override;

	static SGUIGroupMaster* createRetain();
	void init();
	void addMasterGroup(SGUIMasterGroup* group);
	void forEach(const SGActionFn<SGUIMasterGroup*>& action);
private:
	SGUIGroupInfo* m_pInfo;
	SGVector<SGUIMasterGroup*> m_vMasterGroupList;
};

