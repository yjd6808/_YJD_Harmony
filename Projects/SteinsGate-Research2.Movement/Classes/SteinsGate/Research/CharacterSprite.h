/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 3:59:28 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/Engine.h>
#include <SteinsGate/Research/CharacterPartSprite.h>
#include <SteinsGate/Research/Enums.h>

#include <JCore/Tuple.h>

class CharacterSprite : public cocos2d::Sprite
{
public:
	CharacterSprite(CharacterType_t characterType)
		: cocos2d::Sprite()
		, m_pCanvas(nullptr)
		, m_eDirection(SpriteDirection::Right)
		, m_eCharacterType(characterType)
		, m_eMotionState(MotionState::IdleBreath) {}
public:
	static CharacterSprite* create(CharacterType_t characterType);
public:
	bool init() override;
	void update(float delta) override;
public:
	void changePart(CharacterPartType_t partType, int imgIndex);
	CharacterPartSprite* getPart(CharacterPartType_t partType) { return m_vParts[partType]; }
	int getCharacterType() { return m_eCharacterType;  }
	
	SpriteDirection_t getDirection() { return m_eDirection; }
	MotionState_t getMotionState() { return m_eMotionState; }

	void setForwardDirection();
	void setBackwardDirection();
	void setCanvasPosition(const cocos2d::Vec2& pos);
	cocos2d::Vec2 getCanvasPosition();

	void setIdle();
	void setWalk();
	void setRunning();
	void setSlide(SpriteDirection_t direciton, cocos2d::CallFunc* slideBegin, cocos2d::CallFunc* slideEnd);

	cocos2d::Node* getCanvas() { return m_pCanvas; }
	cocos2d::DrawNode* getSkinBoundingBox();
public:
	bool isRunningMotion();
	bool isWalkingMotion();
	bool isIdleMotion();
	bool isShootingMotion();
	bool isJumpMotion();

private:
	cocos2d::Node* m_pCanvas;

	JCore::Vector<CharacterPartSprite*> m_vParts;
	JCore::Vector<JCore::Tuple<int, int>> m_vMotionDetailIndexPairs;

	SpriteDirection_t m_eDirection;
	CharacterType_t m_eCharacterType;


	MotionState_t m_eMotionState;
};


