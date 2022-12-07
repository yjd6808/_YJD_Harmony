/*
 * 작성자 : 윤정도
 */

#pragma once

#include <cocos2d.h>
#include <cocos/ui/CocosGUI.h>

#include <JCore/Container/Vector.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Container/ArrayQueue.h>


#include <Common/Command.h>
#include <Common/MemoryLeakDetector.h>
#include <Common/Enum.h>

#include <JNetwork/Packet.h>
#include <JNetwork/Buffer.h>

#include <TF/Network/GameClient.h>


// 간편한 호출을 위해
#define _Client GameClient::GetInstance()

/*
 * 라이브러리 활용을 위한 예제 프로젝트이므로 namespace를 생략하도록 합니다.
 */

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace JNetwork;
using namespace JCore;

#define RECT_IMG_FILENAME				"white_rect.png"
#define CIRCLE_IMG_FILENAME				"white_circle.png"
#define TRANSPARENT_RECT_IMG_FILENAME	"blank.png"
#define FONT_PATH_DNF					"fonts/dffont.ttf"	// 던파 폰트
#define FONT_PATH_DEFAULT				"fonts/dffont.ttf"	// 기존 폰트
#define CIRCLE_BIG_IMG_FILENAME			"white_circle_big.png"
#define CIRCLE_BIG_IMG_SIZE				300.0f

// 높이는 무조건 너비의 2배가 되도록 할 것
#define TANK_WIDTH	30.0f
#define TANK_HEIGHT	60.0f

