/*
 * 작성자 : 윤정도
 */

#include <SteinsGate/Scenes/Test/LoginLayer.h>

#include "LoginScene.h"

using namespace cocos2d;
using namespace cocos2d::ui;

Sprite* a;
constexpr float FPS1_v = 1.0f / 60.0f;

bool jump_start;

Sprite* s[5];

float x_velocity = 0;
float y_velocity = 0;

float force_x = 0;
float force_y = 0;

constexpr float gravity = 500.0f * FPS1_v;

float ground_y;
float wall_x = 600.0f;
float down_time = 0.0f;
float friction = 800.0f;

Vec2 start_pos;

extern Camera* default_camera;
bool LoginLayer::init() {

	if (!Layer::init()) {
		return false;
	}
	// 아직 씬이 다 초기화가 안된 상태라서 Default 카메라가 세팅이 안되있다. ㄷㄷ
	// Direction::getInstance()->getRunningScene() 이게 안먹힘
	a = Sprite::create("rect1.png");
	a->setColor(Color3B::GRAY);
	a->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	a->setPosition({ 300, 300.0f });
	this->addChild(a);

	
	ground_y = 300.0f;

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(LoginLayer::onKeyPressed2, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);


	EventListenerMouse* mouse =  EventListenerMouse::create();
	mouse->onMouseDown = CC_CALLBACK_1(LoginLayer::onMouseDown, this);
	getEventDispatcher()->addEventListenerWithFixedPriority(mouse, 1);

	Director::getInstance()->getScheduler()->schedule([this](float dt) {
	Vec2 curPos = a->getPosition();

	// 현재 a에게 가해진 힘의 크기가 0보다 큰 경우
	curPos.x += x_velocity * FPS1_v;
	curPos.y += y_velocity * FPS1_v;

	y_velocity -= gravity;

	// y좌표가 지면에 닫은 경우 마찰력 적용
	if (curPos.y <= ground_y + 0.01f) {

		// 오른쪽으로 밀리는 경우 왼쪽 방향으로 마찰력
		if (x_velocity > 0.0f) {
			x_velocity -= friction * FPS1_v;

			// 이때 음수로 전환된 경우 완전히 멈춘 경우임
			if (x_velocity <= 0.0f) {
				x_velocity = 0.0f;
			}
		}
		else if (x_velocity < 0.0f) { // 왼쪽으로 밀리는 경우 오른쪽 방향으로 마찰력 
			x_velocity += friction * FPS1_v;

			// 이때 양수로 전환된 경우 완전히 멈춘 경우임
			if (x_velocity >= 0.0f) {
				x_velocity = 0.0f;
			}
		}
	}

	
	// 공중에 뜬 상태에서 하락중(jump_velocity가 음수로 전환된 시점)일 때 하락시간을 계산한다.
	if (curPos.y > ground_y && y_velocity <= 0.0f) {
		down_time += dt;
	}

	// 공중에 뜬 상태이고 벽에 부딫힌 경우(600.0f 벽기준) 그리고 아직 잔여힘이 남은경우
	if (curPos.y > 0 && curPos.x >= 600.0f && x_velocity > 0) {
		// 반대방향으로 힘을 준다.
		x_velocity *= (-1.0f / 2);	// 벽에 부딫히면 감소시킴
	}

	// 지면에 닫은 경우
	if (curPos.y <= ground_y) {

		// 떨어진 시간 0.4f이상인 경우
		if (down_time >= 0.4f) {

			// 반탄력 느낌으로 초기 공중에 떴을 때의 1/4정도의 힘으로 다시 y에게 힘을 가함
			y_velocity = force_y / 4;

			// 다시 튕길 수 있기때문에 그전 힘의 값을 저장해놓음
			force_y = y_velocity;

			// 다시 올라갔다가 떨어지는 시간을 측정하기 위함
			down_time = 0.0f;
		} else {
			if (x_velocity <= 0.0f) {
				force_x = 0.0f;
			}
		}
		
		a->setPosition(curPos.x, ground_y);
	} else {
		a->setPosition(curPos);
	}

	

	}, this, 1/60.0f, true, "fsefe");
	return true;

	
}
bool bKeyPressed = false;




void LoginLayer::onKeyPressed2(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_A) {
		if (a->getPositionY() > ground_y) {
			// 공중에 뜬상태에서 쳐맞으면 x축 이속 엄청 감소시켜주자.
			x_velocity /= 4.0f;
			y_velocity = 50.0f;
		} else {
			force_x = 200.f;		// 우측방향으로 200만큼 힘을 가함
			force_y = 300.0f;		// 윗 방향으로 300만큼 힘을 가함

			x_velocity = force_x;	// 현재 내 속도 상태를 적용
			y_velocity = force_y;
		}
	}
	if (keyCode == EventKeyboard::KeyCode::KEY_B) {
		Camera::getDefaultCamera()->setPosition(Vec2{100, 100});
		
	}

}

void LoginLayer::onMouseDown(cocos2d::EventMouse* event) {
	auto location = event->getLocation();
	printf("%d, %d\n", (int)location.x, (int)location.y);

}
