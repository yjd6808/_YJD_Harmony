/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 4:00:40 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/FrameTexture.h>

struct FrameEvent {
	int EventType;
	int Id;
};

class CharacterPartSpriteFrame : public cocos2d::SpriteFrame
{
public:
	static CharacterPartSpriteFrame* create();
	~CharacterPartSpriteFrame() override;
public:
	const NpkSpriteRect& getRect();
	bool initWithFrameTexture(const FrameTexturePtr& frameTexture);
	FrameTexture* getFrameTexture() { return m_spFrameTexture.GetPtr(); }
	int getFrameIndex() { return m_spFrameTexture->getFrameIndex(); }
	int getFrameIndexInAnimation() { return m_iFrameIndexInAnimation; }
	bool hasEvent() { return true; }
	FrameEvent* getEvent();
private:
	FrameEvent* m_pEvent = nullptr;	// 테스트
	FrameTexturePtr m_spFrameTexture;
	int m_iFrameIndexInAnimation;	// 애니메이션에서 몇번째 프레임인지

	friend class CharacterSprite;
};