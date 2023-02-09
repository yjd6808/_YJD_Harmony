/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:23:45 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Tutturu.h>
#include <SteinsGate/Enums.h>

class Character : public cocos2d::Sprite
{
public:
	static Character* create();

	bool init() override;
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
public:
	Node* CanvasNode;

	Sprite* Parts[CharacterPartType::Max];
	int PartsZOrder[CharacterPartType::Max];
	int PartsImgIndex[CharacterPartType::Max];
	int FrameIndex;
};
