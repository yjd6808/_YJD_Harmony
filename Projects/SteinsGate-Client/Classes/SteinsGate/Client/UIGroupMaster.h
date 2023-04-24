/*
 * 작성자: 윤정도
 * 생성일: 3/20/2023 9:26:21 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIMasterGroup.h>


class UIGroupMaster : public cocos2d::Ref
{
public:
	UIGroupMaster(UIGroupInfo* info);
	~UIGroupMaster() override;

	static UIGroupMaster* createRetain();
	void init();
	void addMasterGroup(UIMasterGroup* group);
	void forEach(const SGActionFn<UIMasterGroup*>& action);
private:
	UIGroupInfo* m_pInfo;
	SGVector<UIMasterGroup*> m_vMasterGroupList;
};

