/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:24:57 PM
 * =====================
 *
 */


#include "Character.h"

#include <SteinsGate/Common/Core.h>
#include <SteinsGate/Common/Core/Npk/NpkSpriteRect.h>

#include <SteinsGate/PackLoader.h>

using namespace cocos2d;

Character* Character::create() {
	auto c = new (std::nothrow) Character();

	if (c && c->init()) {
		c->autorelease();
		return c;
	}
	CC_SAFE_DELETE(c);
	return nullptr;
}

bool Character::init() {
	if (!Sprite::init())
		return false;

	PartsZOrder[CharacterPartType::Skin] = 0;
	PartsZOrder[CharacterPartType::Pants] = 2;
	PartsZOrder[CharacterPartType::Shoes] = 1;
	PartsZOrder[CharacterPartType::Coat] = 5;
	PartsZOrder[CharacterPartType::Hair] = 2;
	PartsZOrder[CharacterPartType::Face] = 1;
	PartsZOrder[CharacterPartType::Cap] = 3;
	PartsZOrder[CharacterPartType::Belt] = 6;
	PartsZOrder[CharacterPartType::Neck] = 4;
	PartsZOrder[CharacterPartType::Weapon] = 7;

	PartsImgIndex[CharacterPartType::Skin] = 1;
	PartsImgIndex[CharacterPartType::Pants] = 0;
	PartsImgIndex[CharacterPartType::Shoes] = 0;
	PartsImgIndex[CharacterPartType::Coat] = 0;
	PartsImgIndex[CharacterPartType::Hair] = 0;
	PartsImgIndex[CharacterPartType::Face] = 4;
	PartsImgIndex[CharacterPartType::Cap] = 27;
	PartsImgIndex[CharacterPartType::Belt] = 8;
	PartsImgIndex[CharacterPartType::Neck] = 0;
	PartsImgIndex[CharacterPartType::Weapon] = 1;

	FrameIndex = 50;

	// 텍스쳐는 앵커 포인터를 건들면 안된다.
	PackLoader* loader = PackLoader::GetInstance();

	Sprite* part = Sprite::create("rect.png");
	part->setAnchorPoint(Vec2::ZERO);
	part->setOpacity(125);
	this->addChild(part, 0);
	for (int i = 0; i < CharacterPartType::Max; ++i) {

		NpkSpriteRect rect;
		Sprite* part = Sprite::create();
		auto tex = loader->CreateTexture(i, PartsImgIndex[i], FrameIndex, rect);
		part->initWithTexture(tex);							// 앵커포인트랑, 포지션 초기화됨!! 이거땜에 엄청해맸다. Cocos2d-x 숙련도 부족...
		part->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);		// Npk 파일상의 좌표계로 전환(좌상단이 0, 0임)
		part->setPosition(
			(float)rect.X	   - (float)rect.FrameWidth / 2, 
			(float)rect.Y * -1 + (float)rect.FrameHeight / 2);


		this->addChild(part, PartsZOrder[i]);
		Parts[i] = part;
	}
	EventListenerKeyboard* keyboardListener = cocos2d::EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Character::onKeyPressed, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	return true;
}

void Character::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_ENTER: {}
		case EventKeyboard::KeyCode::KEY_A: {}
		case EventKeyboard::KeyCode::KEY_S: {}
	}
}
