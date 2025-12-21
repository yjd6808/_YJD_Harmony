/*
 * 작성자: 윤정도
 * 생성일: 3/20/2023 9:26:21 PM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/UIRootGroup.h>

class UIRootGroupManager : public cocos2d::Ref
{
public:
	UIRootGroupManager(UIGroupInfo* _pInfo);
	~UIRootGroupManager() override;

	static UIRootGroupManager* CreateRetain();

	void Init();
	void AddRootGroup(UIRootGroup* _pGroup);
	void ForEach(const SGActionFn<UIRootGroup*>& _action);

private:
	UIGroupInfo* pInfo_;
	SGVector<UIRootGroup*> rootGroupList_;
};
