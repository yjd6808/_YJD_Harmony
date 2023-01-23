/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:19 PM
 * =====================
 *
 */


#include "SGActorPartSprite.h"

#include <SteinsGate/Common/Engine/RectPoly.h>
#include <SteinsGate/Research/SGActorSprite.h>
#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGGlobal.h>

USING_NS_CC;
USING_NS_JC;


SGActorPartSprite* SGActorPartSprite::create(
	int partIndex, 
	int frameCount, 
	SGActorSprite* actor, 
	SGNode* canvas, 
	SGDrawNode* boundingBox, 
	SGActorPartSpriteData* partData, 
	SGVector<SGAnimationInfo*>& animations
)
{
	SGActorPartSprite* pPartSprite = new SGActorPartSprite(partIndex, frameCount, actor, canvas, boundingBox, partData, animations);

	if (pPartSprite && pPartSprite->init()) {
		pPartSprite->autorelease();
		return pPartSprite;
	}

	DeleteSafe(pPartSprite);
	return nullptr;
}

SGActorPartSprite::SGActorPartSprite(
	int partIndex,
	int frameCount,
	SGActorSprite* actor,
	SGNode* canvas,
	SGDrawNode* boundingBox,
	SGActorPartSpriteData* partData, 
	SGVector<SGAnimationInfo*>& animations)
		: m_iPartIndex(partIndex)
		, m_pActorSprite(actor)
		, m_pCanvas(canvas)
		, m_pPartData(partData)
		, m_refAnimationInfoList(animations)
		, m_pBoundingBox(boundingBox)
		, m_pRunningAnimation(nullptr)
		, m_AnimationMap(animations.Size())
		, m_vAnimationList(animations.Size())
		, m_vFrames(frameCount, nullptr)	
{}

SGActorPartSprite::~SGActorPartSprite() {
	m_AnimationMap.Values().Extension().ForEach([](SGActorPartAnimation* animation) { CC_SAFE_RELEASE(animation); });

	for (int i = 0; i < m_vFrames.Size(); ++i) {
		CC_SAFE_RELEASE(m_vFrames[i]);
	}
}

bool SGActorPartSprite::init() {

	if (!Sprite::initWithTexture(SGGlobal::getInstance()->getDefaultTexture()))
		return false;

	SGImagePack* pImgPack = SGImagePackManager::getInstance()->getPack(m_pPartData->NpkIndex);

	for (int i = 0; i < m_vFrames.Size(); ++i) {
		m_vFrames[i] = pImgPack->createFrameTextureRetain(m_pPartData->ImgIndex, i);

		if (m_vFrames[i] == nullptr) {
			m_vFrames[i] = SGSpriteFrameTexture::createDefaultTextureRetain();
		}

		m_vFrames[i]->retain();
	}

	for (int i = 0; i < m_refAnimationInfoList.Size(); ++i) {
		SGAnimationInfo* pAnimationInfo = m_refAnimationInfoList[i];
		SGActorPartAnimation* pPartAnimation = SGActorPartAnimation::create(this, pAnimationInfo, m_vFrames);
		pPartAnimation->constructFrames(m_pPartData->NpkIndex, m_pPartData->ImgIndex);
		pPartAnimation->retain();
		m_AnimationMap.Insert(pAnimationInfo->Code, pPartAnimation);
		m_vAnimationList.PushBack(pPartAnimation);
	}

	return true;
}

void SGActorPartSprite::update(float dt) { 
	if (m_pRunningAnimation) {
		m_pRunningAnimation->update(dt);
	}

	if (SGGlobal::getInstance()->isBodyBoundingBoxDrawMode())
		m_pBoundingBox->setOpacity(255);
	else
		m_pBoundingBox->setOpacity(0);
}

void SGActorPartSprite::runAnimation(int code) {
	DebugAssertMessage(m_AnimationMap.Exist(code), "해당 코드의 애니메이션이 없습니다.");
	SGActorPartAnimation* pNextAnimation = m_AnimationMap[code];

	//요청한 애니메이션이 동일한 애니메이션인 경우
	if (m_pRunningAnimation == pNextAnimation) {
		m_pRunningAnimation->setLoopSequence();
		return;
	}

	m_pRunningAnimation = pNextAnimation;
	m_pRunningAnimation->run();
}

void SGActorPartSprite::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	m_pActorSprite->onAnimationBegin(animation, texture);
}

void SGActorPartSprite::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	m_pActorSprite->onAnimationEnd(animation, texture);
}

void SGActorPartSprite::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {


	const NpkSpriteRect& rect = texture->getRect();
	const int iFrameIndex = texture->getFrameIndex();
	const int iFrameIndexInAnimation = animation->getFrameIndexInAnimation();

	float fFrameWidth = texture->getFrameWidthF();
	float fFrameHeight = texture->getFrameHeightF();
	
	m_pCanvas->setPosition(-fFrameWidth / 2, -fFrameHeight / 2);
	m_pCanvas->setContentSize(fFrameWidth, fFrameHeight);

	// Coordinate-System(좌표계) 변경
	float fWidth = rect.GetWidthF();
	float fHeight = rect.GetHeightF();
	float fAdjustX = rect.X;
	float fAdjustY = fFrameHeight - rect.Y - fHeight;

	this->setContentSize({ fWidth, fHeight });
	this->setPosition(fAdjustX, fAdjustY);

	// 바디 파츠만 그려주자.
	if (m_iPartIndex == 0) {
 		RectPoly poly = RectPoly::createFromLeftBottom({0, 0}, { fWidth, fHeight });

		m_pBoundingBox->clear();
		m_pBoundingBox->setContentSize({ fWidth, fHeight });
		m_pBoundingBox->drawPolygon(poly.source(), 4, {}, 1, Color4F::WHITE);

		//Log("%d 프레임 종료\n", m_pRunningAnimation->getFrameIndex());
	}

	m_pActorSprite->onFrameBegin(animation, texture);
	// Log("[%d 파츠] - %d 프레임 종료\n", m_iPartIndex, m_pRunningAnimation->getFrameIndex());
}

void SGActorPartSprite::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	m_pActorSprite->onFrameEnd(animation, texture);
}

void SGActorPartSprite::onCustomFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
}

void SGActorPartSprite::onCustomFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
}