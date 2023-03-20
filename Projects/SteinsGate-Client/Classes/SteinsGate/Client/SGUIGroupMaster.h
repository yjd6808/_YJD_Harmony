/*
 * 작성자: 윤정도
 * 생성일: 3/20/2023 9:26:21 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGUIGroup.h>

class SGUIGroupMaster : public SGUIGroup
{
public:
	SGUIGroupMaster(SGUIGroupInfo* info);
	~SGUIGroupMaster() override;

	static SGUIGroupMaster* createRetain();
	void addUIGroup(const SGUIGroupElemInfo& groupElemInfo);
	bool init() override;
	bool isGroupMaster() const override { return true; }
};

