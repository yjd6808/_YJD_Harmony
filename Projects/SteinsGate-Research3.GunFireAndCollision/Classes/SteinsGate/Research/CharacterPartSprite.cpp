/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 4:02:30 AM
 * =====================
 *
 */

#include "CharacterPartSprite.h"

#include <SteinsGate/Research/Config.h>
#include <SteinsGate/Research/CharacterSprite.h>

#include "World.h"

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
	m_pBoundingBox->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	this->addChild(m_pBoundingBox);

	return true;
}

void CharacterPartSprite::onFrameBegin(CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame)
{
	const NpkSpriteRect& rect = frame->getRect();
	const int iFrameIndex = frame->getFrameIndex();
	const int iFrameIndexInAnimation = frame->getFrameIndexInAnimation();


	// Coordinate-System(좌표계) 변경
	float fWidth = rect.GetWidthF();
	float fHeight = rect.GetHeightF();
	float fAdjustX = rect.X;
	float fAdjustY = m_pParent->getCanvas()->getContentSize().height - rect.Y - fHeight;

	this->setPosition(fAdjustX, fAdjustY);
	
	if (m_ePartType == CharacterPartType::Skin) {
		Vec2 polygon[4];
		polygon[0] = Vec2(0.0f, fHeight);
		polygon[1] = Vec2(fWidth, fHeight);
		polygon[2] = Vec2(fWidth, 0.0f);
		polygon[3] = Vec2(0.0f, 0.0f);

		m_pBoundingBox->clear();
		m_pBoundingBox->setContentSize({ fWidth, fHeight });
		m_pBoundingBox->drawPolygon(polygon, 4, {}, 1, Color4F::WHITE);
	}	


	auto pWorld = World::getInstance();
	auto pPlayer = pWorld->getPlayer();
	auto pActionManager = pPlayer->getPlayerActionManager();

	// 파츠가 10개 부착되어있으면 아래 함수들은 10번씩 수행됨
	// 제일 처음 시작하는 경우
	if (iFrameIndexInAnimation == 0) {
		pActionManager->onAnimateBegin(m_pParent, animate, frame);
		pActionManager->onFrameBegin(m_pParent, animate, frame);
	} else {
		pActionManager->onFrameEnd(m_pParent, animate, animate->getFrameAt(iFrameIndexInAnimation - 1));
		pActionManager->onFrameBegin(m_pParent, animate, frame);
	}

	// 모션 수행중인지 여부 업데이트를 위함
	if (m_pParent->getPart(CharacterPartType::Skin) && iFrameIndexInAnimation == 0)
		m_pParent->onAnimateBegin(animate->getMotionState());

}

void CharacterPartSprite::onAnimateEnd(CharacterPartAnimate* animate, CharacterPartSpriteFrame* lastFrame) {
	auto pActionManager = World::getInstance()->getPlayer()->getPlayerActionManager();
	pActionManager->onFrameEnd(m_pParent, animate, lastFrame);
	pActionManager->onAnimateEnd(m_pParent, animate, lastFrame);

	if (m_pParent->getPart(CharacterPartType::Skin))
		m_pParent->onAnimateEnd(animate->getMotionState());
}

void CharacterPartSprite::runAnimation(int motionState) {
	stopAllActions();
	runAction(m_pAnimateMap[motionState]);
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


