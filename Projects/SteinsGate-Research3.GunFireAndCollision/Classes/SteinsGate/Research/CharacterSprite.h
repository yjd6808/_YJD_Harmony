/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 3:59:28 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/Tutturu.h>
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
		, m_iMotionState(MotionState::IdleBreath) {}
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
	int getMotionState() { return m_iMotionState; }
	cocos2d::Vec2 getWorldPosition();

	void setForwardDirection();
	void setBackwardDirection();
	void setCanvasPosition(const cocos2d::Vec2& pos);
	cocos2d::Vec2 getCanvasPosition();

	void onAnimateBegin(int motionState);
	void onAnimateEnd(int motionState);

	void runMotion(int motionState1);
	void runMotionRepeat(int motionState1);
	void runMotion(int motionState1, int motionState2);
	void runMotion(int motionState1, int motionState2, int motionState3);
	void runMotion(int motionState1, int motionState2, int motionState3, int motionState4);
	void setIdle();
	void setWalk();
	void setRunning();
	void setSlide(SpriteDirection_t direciton);

	cocos2d::Node* getCanvas() { return m_pCanvas; }
	cocos2d::DrawNode* getSkinBoundingBox() { return m_vParts[CharacterPartType::Skin]->getDrawNode(); }
	cocos2d::Size getCanvasSize() { return m_pCanvas->getContentSize(); }
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

	int m_iMotionState;
};


