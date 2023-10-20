/*
 * 작성자: 윤정도
 * 생성일: 10/9/2023 4:28:58 PM
 * =====================
 *
 */


#pragma once

#include <TF/Common/PchBase.h>

#include <TF/Client/Const.h>

#include <TF/Client/Util/Color.h>

#include <TF/Client/Net/NetCore.h>
#include <TF/Client/Net/Host/GameClient.h>

#include <TF/Client/Game/UI/PopUp.h>
#include <TF/Client/Game/UI/TextButton.h>
#include <TF/Client/Game/UI/ColoredListView.h>
#include <TF/Client/Game/UI/ChatBox.h>

#include <TF/Client/Game/Contents/Room.h>

#include <cocos2d.h>
#include <cocos/ui/CocosGUI.h>

#define USING_NS_CCUI	using namespace cocos2d::ui

namespace Core
{
	inline ::NetCore* NetCore;
	inline ::GameClient* GameClient;
	inline ::Room* Room;
}

void InitializeGameCore();
void FinalizeGameCore();