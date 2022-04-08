/*
 * �ۼ��� : ������
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


// ������ ȣ���� ����
#define _Client GameClient::GetInstance()

/*
 * ���̺귯�� Ȱ���� ���� ���� ������Ʈ�̹Ƿ� namespace�� �����ϵ��� �մϴ�.
 */

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace JNetwork;
using namespace JCore;

#define RECT_IMG_FILENAME				"white_rect.png"
#define CIRCLE_IMG_FILENAME				"white_circle.png"
#define TRANSPARENT_RECT_IMG_FILENAME	"blank.png"
#define FONT_PATH_DNF					"fonts/dffont.ttf"	// ���� ��Ʈ
#define FONT_PATH_DEFAULT				"fonts/dffont.ttf"	// ���� ��Ʈ

// ���̴� ������ �ʺ��� 2�谡 �ǵ��� �� ��
#define TANK_WIDTH	30.0f
#define TANK_HEIGHT	60.0f

