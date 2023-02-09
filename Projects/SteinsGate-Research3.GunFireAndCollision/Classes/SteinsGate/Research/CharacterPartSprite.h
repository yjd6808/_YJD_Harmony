﻿/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 4:02:18 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/Enums.h>
#include <SteinsGate/Research/CharacterPartAnimate.h>

class CharacterSprite;
class CharacterPartSprite : public cocos2d::Sprite
{

public:
	static CharacterPartSprite* create(CharacterSprite* parent, CharacterPartType_t partType, int frameCount);

	CharacterPartSprite(CharacterSprite* parent, CharacterPartType_t partType, int frameCount)
		: cocos2d::Sprite()
		, m_pParent(parent)
		, m_ePartType(partType)
		, m_vFrames(frameCount) {}
	~CharacterPartSprite() override;
public:
	bool init() override;
	void onFrameBegin(CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame);
	void onAnimateEnd(CharacterPartAnimate* animate, CharacterPartSpriteFrame* lastFrame);
	void runAnimation(int motionState);

	void runSequence(cocos2d::Sequence* sequence, bool repeat);
	void runSequenceOnce(cocos2d::Sequence* sequence);
	void runSequenceRepeat(cocos2d::Sequence* sequence);

	CharacterPartAnimate* getAnimation(int motionState) { 
		return m_pAnimateMap[motionState];
	}

	void addAnimation(int state, CharacterPartAnimate* animate) {
		m_pAnimateMap.Insert(state, animate);
	}

	CharacterPartType_t getPartType() { return m_ePartType; }
	CharacterPartSpriteFrame* getFrame(int index) { return m_vFrames[index]; }
	cocos2d::DrawNode* getDrawNode() { return m_pBoundingBox; }
	
	
private:
	cocos2d::DrawNode* m_pBoundingBox;

	// (상태, 애니메이션)
	CharacterSprite* m_pParent;
	CharacterPartType_t m_ePartType;
	
	JCore::HashMap<int, CharacterPartAnimate*> m_pAnimateMap;	
	JCore::Vector<CharacterPartSpriteFrame*> m_vFrames;
};