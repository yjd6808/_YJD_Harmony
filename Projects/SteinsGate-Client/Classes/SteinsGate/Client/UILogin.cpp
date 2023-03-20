/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:07:34 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "UILogin.h"
#include "SGUIGroup.h"

#include <SteinsGate/Client/SGUIDefine.h>
#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGImagePackManager.h>

USING_NS_CC;
USING_NS_JC;


UILogin::UILogin(SGUIGroup* groupMaster, SGUIGroupInfo* groupInfo)
	: SGUIGroup(groupMaster, groupInfo)
{
}


bool UILogin::init() {

	if (!SGUIGroup::init()) {
		return false;
	}

	return true;
}


