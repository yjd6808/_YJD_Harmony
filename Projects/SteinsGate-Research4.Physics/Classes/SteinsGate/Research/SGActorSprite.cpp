/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:44 PM
 * =====================
 *
 */


#include "SGActorSprite.h"

#include "SGActor.h"
#include "SGGlobal.h"
#include "SGImagePackManager.h"

USING_NS_CC;
USING_NS_JC;

SGActorSprite::SGActorSprite(
	SGActor* actor, 
	const SGActorSpriteDataPtr& actorData, 
	bool attachPartsBoundingBoxToCanvas
)
	: m_bPartsBoundingBoxAttachedToCanvas(attachPartsBoundingBoxToCanvas)
	, m_pActor(actor)
	, m_spActorData(actorData)
	, m_vPartsCanvas(actorData->Parts.Size(), nullptr)
	, m_vParts(actorData->Parts.Size(), nullptr)
	, m_vPartsBoundingBox(actorData->Parts.Size(), nullptr)
	, m_eDirection(SpriteDirection::Right)
{
}

SGActorSprite* SGActorSprite::create(
	SGActor* actor, const 
	SGActorSpriteDataPtr& actorData, 
	bool attachPartsBoundingBoxToCanvas)
{
	SGActorSprite* pSprite = new SGActorSprite(actor, actorData, attachPartsBoundingBoxToCanvas);

	if (pSprite && pSprite->init()) {
		pSprite->autorelease();
		return pSprite;
	}

	DeleteSafe(pSprite);
	return pSprite;
}

bool SGActorSprite::init() {
	if (!Sprite::init())
		return false;

	SGVector<SGActorPartSpriteData>& vPartsData = m_spActorData->Parts;
	vPartsData.Sort([](SGActorPartSpriteData& lhs, SGActorPartSpriteData& rhs) { return lhs.ZOrder < rhs.ZOrder; });

	// 바디 파츠 기준으로 전체 프레임수를 얻는다.
	int iFrameCount = SGImagePackManager::getInstance()
		->getPack(vPartsData[0].NpkIndex)
		->getSpriteCount(vPartsData[0].ImgIndex);

	for (int i = 0; i < vPartsData.Size(); ++i) {
		
		m_vPartsCanvas[i] = SGSprite::create("rect.png");
		m_vPartsCanvas[i]->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		m_vPartsCanvas[i]->setCascadeOpacityEnabled(false);
		m_vPartsCanvas[i]->setOpacity(0);
		if (i == 0)
			m_vPartsCanvas[i]->setOpacity(80);
		this->addChild(m_vPartsCanvas[i], i);	// 정렬된 순서대로 ZOrder 반영

		m_vPartsBoundingBox[i] = SGDrawNode::create();
		m_vParts[i] = SGActorPartSprite::create(
			i,
			iFrameCount,
			this , 
			m_vPartsCanvas[i],
			m_vPartsBoundingBox[i],
			&vPartsData[i], 
			m_spActorData->Animations
		);

		m_vParts[i]->setAnchorPoint(Vec2::ZERO);
		m_vPartsCanvas[i]->addChild(m_vParts[i]);

		if (m_bPartsBoundingBoxAttachedToCanvas)
			m_vPartsCanvas[i]->addChild(m_vPartsBoundingBox[i]);
		else
			m_vParts[i]->addChild(m_vPartsBoundingBox[i]);
	}

	return true;
}

void SGActorSprite::update(float dt) {
	for (int i = 0; i < m_vParts.Size(); ++i) {
		m_vParts[i]->update(dt);
	}
}

void SGActorSprite::runAnimation(int code) {
	for (int i = 0; i < m_vParts.Size(); ++i) {
		m_vParts[i]->runAnimation(code);
	}
}


// 무조건 마지막 인덱스 기준으로 해줘야함.
// 0 ~ 8번 순서대로 업데이트를 진행하는데
// 만약 0번 인덱스의 onFrameEnd 호출 후 뒤의 애니메이션 상태를 바꿔버리면
// 해당 프레임때 곧바로 적용되어버리기 1 ~ 7번 프레임은 바뀐 상태로 업데이트를 수행함. 때문에 프레임이 1칸씩 밀릴 수도 있음
void SGActorSprite::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (animation->getPartIndex() == m_vParts.Size() - 1)
		m_pActor->onFrameBegin(animation, texture);
}

void SGActorSprite::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (animation->getPartIndex() == m_vParts.Size() - 1)
		m_pActor->onFrameEnd(animation, texture);
}

void SGActorSprite::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (animation->getPartIndex() == m_vParts.Size() - 1)
		m_pActor->onAnimationBegin(animation, texture);
}

void SGActorSprite::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (animation->getPartIndex() == m_vParts.Size() - 1)
		m_pActor->onAnimationEnd(animation, texture);
}

void SGActorSprite::setSpriteDirection(SpriteDirection_t direction) {
	if (direction == SpriteDirection::Right)
		setForwardDirection();
	else if (direction == SpriteDirection::Left)
		setBackwardDirection();
}


void SGActorSprite::setForwardDirection() {
	setRotationSkewY(0);
	m_eDirection = SpriteDirection::Right;
}

void SGActorSprite::setBackwardDirection() {
	setRotationSkewY(180.0f);
	m_eDirection = SpriteDirection::Left;
}

SGActorPartSprite* SGActorSprite::getBodyPart() {
	return m_vParts[0];
}

SGNode* SGActorSprite::getBodyCanvas() {
	return m_vParts[0]->getCanvas();
}

SGSize SGActorSprite::getBodyCanvasSize() {
	return m_vParts[0]->getCanvas()->getContentSize();
}

SGSize SGActorSprite::getBodyPartSize() {
	return m_vParts[0]->getBoundingBox()->getContentSize();
}

SGVec2 SGActorSprite::getBodyPartPosition() {
	return m_vParts[0]->getPosition();
}

SpriteDirection_t SGActorSprite::getSpriteDirection() {
	return m_eDirection;
}
