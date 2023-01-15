/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 4:02:30 AM
 * =====================
 *
 */

#include "CharacterPartSprite.h"

#include <SteinsGate/Research/Config.h>
#include <SteinsGate/Research/CharacterSprite.h>

using namespace cocos2d;

CharacterPartSprite* CharacterPartSprite::create(CharacterSprite* parent, CharacterPartType_t partType, int frameCount)
{
	CharacterPartSprite* s = new (std::nothrow) CharacterPartSprite(parent, partType, frameCount);

	if (s && s->init()) {
		return s;
	}

	CC_SAFE_DELETE(s);
	return nullptr;
}



CharacterPartSprite::~CharacterPartSprite()
{
	m_pAnimateMap.Values().Extension().ForEach([](CharacterPartAnimate* animate) {
		delete animate;
	});
}

bool CharacterPartSprite::init()
{
	// 필수로 해줘야함, 이거땜에 1시간 날림;
	if (!Sprite::init()) 
		return false;

	for (int j = 0; j < CharacterFrameCount_v[m_pParent->getCharacterType()]; ++j) {
		m_vFrames.PushBack(CharacterPartSpriteFrame::create());
	}

	m_pBoundingBox = DrawNode::create();
	m_pBoundingBox->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pBoundingBox);

	return true;
}

void CharacterPartSprite::onFrameBegin(CharacterPartSpriteFrame* frame)
{
	const NpkSpriteRect& rect = frame->getRect();
	const int iFrameIndex = frame->getFrameIndex();
	const int iFrameIndexInAnimation = frame->getFrameIndexInAnimation();

	float fAdjustX = (float)rect.X		- CharacterPartFrameContentWidth_v / 2.0f;
	float fAdjustY = (float)rect.Y * -1 + CharacterPartFrameContentHeight_v / 2.0f;
	float fWidth = rect.GetWidthF();
	float fHeight = rect.GetHeightF();

	CharacterPartSprite* pPart = m_pParent->getPart(m_ePartType);
	pPart->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	pPart->setPosition(fAdjustX, fAdjustY);

	printf("%d", m_ePartType);

	if (m_ePartType == CharacterPartType::Skin) {
		Vec2 polygon[4];

		polygon[0] = Vec2(0.0f, fHeight);
		polygon[1] = Vec2(fWidth, fHeight);
		polygon[2] = Vec2(fWidth, 0.0f);
		polygon[3] = Vec2(0.0f, 0.0f);

		m_pBoundingBox->clear();
		m_pBoundingBox->drawPolygon(polygon, 4, {}, 1, Color4F::WHITE);
	}	

	// 제일 처음 시작하는 경우
	if (m_ePartType == CharacterPartType::Begin) {
	}


	if (m_ePartType == CharacterPartType::End && iFrameIndexInAnimation == m_vFrames.Size() - 1) {
	}
}

void CharacterPartSprite::runAnimation(MotionState_t stateIndex) {
	stopAllActions();
	runAction(m_pAnimateMap[stateIndex]);
}

void CharacterPartSprite::runSequence(cocos2d::Sequence* sequence, bool repeat) {
	if (repeat) {
		runSequenceRepeat(sequence);
		return;
	}

	runSequenceOnce(sequence);
}

void CharacterPartSprite::runSequenceOnce(cocos2d::Sequence* sequence) {
	stopAllActions();
	runAction(sequence);
}

void CharacterPartSprite::runSequenceRepeat(cocos2d::Sequence* sequence) {
	stopAllActions();
	runAction(RepeatForever::create(sequence));
}


