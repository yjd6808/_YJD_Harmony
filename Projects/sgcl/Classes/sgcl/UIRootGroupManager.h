/*
 * 작성자: 윤정도
 * 생성일: 3/20/2023 9:26:21 PM
 * =====================
 *
 */

#pragma once

#include <sgcl/UIRootGroup.h>

class UIRootGroupManager : public cocos2d::Ref
{
public:
	UIRootGroupManager(UIGroupInfo* _pInfo);
	~UIRootGroupManager() override;

	static UIRootGroupManager* CreateRetain();

	void Init();
	void AddRootGroup(UIRootGroup* _pGroup);
	void ForEach(const jc::Action<UIRootGroup*>& _action);

private:
	UIGroupInfo* pInfo_;
	jc::Vector<UIRootGroup*> rootGroupList_;
};
