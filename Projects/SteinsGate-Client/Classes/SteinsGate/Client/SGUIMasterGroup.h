/*
 * 작성자: 윤정도
 * 생성일: 3/22/2023 12:50:32 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGUIGroup.h>

class SGUIMasterGroup : public SGUIGroup
{
public:
	SGUIMasterGroup(SGUIGroupInfo* groupInfo);

	bool isMasterGroup() override { return true; }
};

