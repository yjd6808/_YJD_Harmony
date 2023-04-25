/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:19 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ActorPartSprite.h"

#include <SteinsGate/Common/RectPoly.h>
#include <SteinsGate/Client/ActorSprite.h>
#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/Global.h>

#include "Actor.h"

USING_NS_CC;
USING_NS_JC;


ActorPartSprite* ActorPartSprite::create(
	int frameCount, 
	ActorSprite* actor, 
	SGNode* canvas, 
	SGDrawNode* boundingBox, 
	const ActorPartSpriteData& partData, 
	SGVector<AnimationInfo*>& animations
)
{
	ActorPartSprite* pPartSprite = dbg_new ActorPartSprite(frameCount, actor, canvas, boundingBox, partData, animations);

	if (pPartSprite && pPartSprite->init()) {
		pPartSprite->autorelease();
		return pPartSprite;
	}

	DeleteSafe(pPartSprite);
	return nullptr;
}

ActorPartSprite::ActorPartSprite(
	int frameCount,
	ActorSprite* actor,
	SGNode* canvas,
	SGDrawNode* boundingBox,
	const ActorPartSpriteData& partData,
	SGVector<AnimationInfo*>& animations)
		: m_pActorSprite(actor)
		, m_pCanvas(canvas)
		, m_PartData(partData)
		, m_refAnimationInfoList(animations)
		, m_pBoundingBox(boundingBox)
		, m_pRunningAnimation(nullptr)
		, m_AnimationMap(animations.Size())
		, m_vAnimationList(animations.Size())
		, m_vFrames(frameCount, nullptr)	
{}

ActorPartSprite::~ActorPartSprite() {
	m_AnimationMap.Values().Extension().ForEach([](ActorPartAnimation* animation) { CC_SAFE_RELEASE(animation); });

	for (int i = 0; i < m_vFrames.Size(); ++i) {
		CC_SAFE_RELEASE(m_vFrames[i]);
	}
}

bool ActorPartSprite::init() {

	if (!Sprite::init())
		return false;

	

	ImagePack* pImgPack = ImagePackManager::Get()->getPack(m_PartData.SgaIndex);

	for (int i = 0; i < m_vFrames.Size(); ++i) {

		m_vFrames[i] = pImgPack->createFrameTexture(m_PartData.ImgIndex, i);

		if (m_vFrames[i] == nullptr) {
			m_vFrames[i] = Global::Get()->getDefaultFrameTexture();
		}

		m_vFrames[i]->retain();
	}

	for (int i = 0; i < m_refAnimationInfoList.Size(); ++i) {
		AnimationInfo* pAnimationInfo = m_refAnimationInfoList[i];
		ActorPartAnimation* pPartAnimation = ActorPartAnimation::create(this, pAnimationInfo, m_vFrames);
		pPartAnimation->constructFrames(m_PartData.SgaIndex, m_PartData.ImgIndex);
		pPartAnimation->retain();
		m_AnimationMap.Insert(pAnimationInfo->Code, pPartAnimation);
		m_vAnimationList.PushBack(pPartAnimation);
	}

	return true;
}

void ActorPartSprite::update(float dt) { 
	if (m_pRunningAnimation) {
		m_pRunningAnimation->update(dt);
	}

	updateBoundingBoxVisibleState();
}

void ActorPartSprite::updateBoundingBoxVisibleState() {
	if (Global::Get()->DrawBodyBoundingBox)
		m_pBoundingBox->setOpacity(255);
	else
		m_pBoundingBox->setOpacity(0);
}

void ActorPartSprite::runAnimation(int code) {
	runAnimation(code, 0);
}

void ActorPartSprite::runAnimation(int code, int startFrameIndexInAnimation) {
	DebugAssertMsg(m_AnimationMap.Exist(code), "해당 코드의 애니메이션이 없습니다.");
	ActorPartAnimation* pNextAnimation = m_AnimationMap[code];

	//요청한 애니메이션이 동일한 애니메이션인 경우
	if (m_pRunningAnimation == pNextAnimation && startFrameIndexInAnimation == 0) {
		m_pRunningAnimation->setLoopSequence();
		return;
	}

	m_pRunningAnimation = pNextAnimation;
	m_pRunningAnimation->run(startFrameIndexInAnimation);

	updateBoundingBoxVisibleState();
}

ActorType_t ActorPartSprite::getActorType() {
	return getActorSprite()->getActor()->getType();
}

void ActorPartSprite::onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	m_pActorSprite->onAnimationBegin(animation, texture);
}

void ActorPartSprite::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	m_pActorSprite->onAnimationEnd(animation, texture);
}

void ActorPartSprite::onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {

	const SgaSpriteRect& rect = texture->getFullRect();
	const int iFrameIndex = texture->getFrameIndex();
	const int iFrameIndexInAnimation = animation->getFrameIndexInAnimation();

	float fFrameWidth = texture->getFrameWidthF();
	float fFrameHeight = texture->getFrameHeightF();


	// Coordinate-System(좌표계) 변경
	float fWidth = (float)rect.Width;
	float fHeight = (float)rect.Height;
	float fAdjustX = (float)rect.X;
	float fAdjustY = fFrameHeight - (float)rect.Y - fHeight;

	bool bProjectile = getActorType() == ActorType::Projectile;

	// 총알은 캔버스가 필요치 않다.
	// 위치도 0, 0 고정임
	if (bProjectile == false) {
		m_pCanvas->setPosition(-fFrameWidth / 2, -fFrameHeight / 2);
		m_pCanvas->setContentSize(fFrameWidth, fFrameHeight);

		this->setContentSize({ fWidth, fHeight });
		this->setPosition(fAdjustX, fAdjustY);
	} 
	
	// 바디 파츠만 그려주자.
	if (m_iPartIndex == 0) {
		SGRect boundingBox = getBoundingBox();

		// initWithTexture 이후 앵커 포인트 초기화 되는것에 대한 처리는
		// SGActorPartAnimation::changeTexture 함수에서 처리했다.
		// 이거 모르면 헤매기 쉬움

		// DrawNode는 앵커 포인터의 영향을 받지 않고
		// 무조건 좌하단에서부터 그린다.
		// 총알, 캔버스, 바운딩박스 모두 0, 0에 위치함
		RectPoly poly = bProjectile ?
			RectPoly::createFromCenter({ 0, 0 }, boundingBox.size) :	// 총알은 0,0에서 그려지므로 센터 기준으로 박스 만듬
			RectPoly::createFromLeftBottom({ 0, 0 }, boundingBox.size);

		m_pBoundingBox->clear();
		m_pBoundingBox->setContentSize({ boundingBox.size });
		m_pBoundingBox->drawPolygon(poly.source(), 4, {}, 1, Color4F::WHITE);
	}

	m_pActorSprite->onFrameBegin(animation, texture);
}

void ActorPartSprite::onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	m_pActorSprite->onFrameEnd(animation, texture);
}

void ActorPartSprite::onCustomFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {
}

void ActorPartSprite::onCustomFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {
}


// runningAnimation과 동일한 상태의 애니메이션으로 변경
void ActorPartSprite::reflectAnimation(ActorPartAnimation* runningAnimation) {
	int iAnimationCode = runningAnimation->getAnimationCode();
	DebugAssertMsg(m_AnimationMap.Exist(iAnimationCode), "기존 액터파츠가 수행중이던 애니메이션을 교체할려고했으나 신규 액터 파츠가 수행할 수 있는 애니메이션목록에 없습니다.");
	ActorPartAnimation* pAnimation = m_AnimationMap[iAnimationCode];

	m_pRunningAnimation = pAnimation;
	m_pRunningAnimation->reflectAnimation(runningAnimation);
}
