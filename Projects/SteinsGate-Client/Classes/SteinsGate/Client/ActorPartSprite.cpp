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

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartSprite* ActorPartSprite::create(
	int _frameCount,
	ActorSprite* _pActor,
	SGNode* _pCanvas,
	SGDrawNode* _pBoundingBox,
	const ActorPartSpriteData& _partData,
	SGVector<AnimationInfo>* _pAnimations
)
{
	ActorPartSprite* pPartSprite = dbg_new ActorPartSprite(_frameCount, _pActor, _pCanvas, _pBoundingBox, _partData, _pAnimations);

	if (pPartSprite && pPartSprite->init())
	{
		pPartSprite->autorelease();
		return pPartSprite;
	}

	JCORE_DELETE_SAFE(pPartSprite);
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartSprite::ActorPartSprite(
	int _frameCount,
	ActorSprite* _pActor,
	SGNode* _pCanvas,
	SGDrawNode* _pBoundingBox,
	const ActorPartSpriteData& _partData,
	SGVector<AnimationInfo>* _pAnimations)
	: actorSprite_(_pActor)
	, partData_(_partData)
	, animationInfoList_(_pAnimations)
	, canvas_(_pCanvas)
	, boundingBox_(_pBoundingBox)
	, runningAnimation_(nullptr)
	, animationMap_(_pAnimations->Size())
	, animationList_(_pAnimations->Size())
	, frames_(_frameCount, nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartSprite::~ActorPartSprite()
{
	animationMap_.ForEachValue([](ActorPartAnimation* _pAnimation) { CC_SAFE_RELEASE(_pAnimation); });

	for (int i = 0; i < frames_.Size(); ++i)
	{
		CC_SAFE_RELEASE(frames_[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ActorPartSprite::init()
{
	if (!Sprite::init())
		return false;

	initFrames();
	initAnimations();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::initFrames()
{
	ImagePack* pImagePack = ImagePackManager::Get()->getPack(partData_.SgaIndex);

	for (int i = 0; i < frames_.Size(); ++i)
	{
		frames_[i] = pImagePack->createFrameTexture(partData_.ImgIndex, i);

		if (frames_[i] == nullptr)
		{
			frames_[i] = Global::Get()->getDefaultFrameTexture();
		}

		frames_[i]->retain();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::initAnimations()
{
	for (int i = 0; i < animationInfoList_->Size(); ++i)
	{
		AnimationInfo& animationInfo = animationInfoList_->At(i);
		ActorPartAnimation* pPartAnimation = ActorPartAnimation::create(this, &animationInfo, frames_);
		pPartAnimation->constructFrames(partData_.SgaIndex, partData_.ImgIndex);
		pPartAnimation->retain();
		animationMap_.Insert(animationInfo.Code, pPartAnimation);
		animationList_.PushBack(pPartAnimation);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::update(float _dt)
{
	if (runningAnimation_)
	{
		runningAnimation_->update(_dt);
	}

	updateBoundingBoxVisibleState();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::updateBoundingBoxVisibleState()
{
	if (Global::Get()->DrawBodyBoundingBox)
		boundingBox_->setOpacity(255);
	else
		boundingBox_->setOpacity(0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::updateAnimationReference(SGVector<AnimationInfo>* _pAnimations)
{
	animationInfoList_ = _pAnimations;
	SGVector<ActorPartAnimation*> updatedAnimationList(animationList_.Size());

	for (int i = 0; i < animationInfoList_->Size(); ++i)
	{
		AnimationInfo& animationInfo = animationInfoList_->At(i);

		if (!animationMap_.Exist(animationInfo.Code))
		{
			DebugAssertMsg(false, "애니메이션 업데이트 중 오류발생 - 현재 파츠에서 %s 애니메이션은 존재하지 않음", animationInfo.Name.Source());
			return;
		}

		ActorPartAnimation* pAnimation = animationMap_[animationInfo.Code];
		pAnimation->setAnimationInfo(&animationInfo);
		updatedAnimationList.PushBack(pAnimation);
	}

	DebugAssert(animationList_.Size() == updatedAnimationList.Size(), "업데이트될 애니메이션의 갯수가 기존과 다릅니다.");
	animationList_ = updatedAnimationList;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::runAnimation(int _code)
{
	runAnimation(_code, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::runAnimation(int _code, int _startFrameIndexInAnimation)
{
	DebugAssertMsg(animationMap_.Exist(_code), "해당 코드의 애니메이션이 없습니다.");
	ActorPartAnimation* pNextAnimation = animationMap_[_code];

	// 요청한 애니메이션이 동일한 애니메이션인 경우
	if (runningAnimation_ == pNextAnimation && _startFrameIndexInAnimation == 0)
	{
		runningAnimation_->setLoopSequence();
		return;
	}

	runningAnimation_ = pNextAnimation;
	runningAnimation_->run(_startFrameIndexInAnimation);

	updateBoundingBoxVisibleState();
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorType_t ActorPartSprite::getActorType()
{
	return getActorSprite()->getActor()->getType();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	actorSprite_->onAnimationBegin(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	actorSprite_->onAnimationEnd(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	const SgaSpriteRect& rect = _pTexture->getFullRect();
	const int frameIndex = _pTexture->getFrameIndex();
	const int frameIndexInAnimation = _pAnimation->getFrameIndexInAnimation();

	float frameWidth = _pTexture->getFrameWidthF();
	float frameHeight = _pTexture->getFrameHeightF();

	// Coordinate-System(좌표계) 변경
	float width = static_cast<float>(rect.Width);
	float height = static_cast<float>(rect.Height);
	float adjustX = static_cast<float>(rect.X);
	float adjustY = frameHeight - static_cast<float>(rect.Y) - height;

	bool isProjectile = getActorType() == ActorType::Projectile;

	// 총알은 캔버스가 필요치 않다.
	// 위치도 0, 0 고정임
	if (!isProjectile)
	{
		canvas_->setPosition(-frameWidth / 2, -frameHeight / 2);
		canvas_->setContentSize(frameWidth, frameHeight);

		this->setContentSize({ width, height });
		this->setPosition(adjustX, adjustY);
	}

	// 바디 파츠만 그려주자.
	if (partIndex_ == 0)
	{
		SGRect boundingBox = getBoundingBox();

		// initWithTexture 이후 앵커 포인트 초기화 되는것에 대한 처리는
		// SGActorPartAnimation::changeTexture 함수에서 처리했다.
		// 이거 모르면 헤매기 쉬움

		// DrawNode는 앵커 포인터의 영향을 받지 않고
		// 무조건 좌하단에서부터 그린다.
		// 총알, 캔버스, 바운딩박스 모두 0, 0에 위치함
		RectPoly poly = isProjectile ?
			RectPoly::createFromCenter({ 0, 0 }, boundingBox.size) :
			RectPoly::createFromLeftBottom({ 0, 0 }, boundingBox.size);

		boundingBox_->clear();
		boundingBox_->setContentSize({ boundingBox.size });
		boundingBox_->drawPolygon(poly.source(), 4, {}, 1, Color4F::WHITE);
	}

	actorSprite_->onFrameBegin(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	actorSprite_->onFrameEnd(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::onCustomFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::onCustomFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::reflectAnimation(ActorPartAnimation* _pRunningAnimation)
{
	int animationCode = _pRunningAnimation->getAnimationCode();
	DebugAssertMsg(animationMap_.Exist(animationCode), "기존 액터파츠가 수행중이던 애니메이션을 교체할려고했으나 신규 액터 파츠가 수행할 수 있는 애니메이션목록에 없습니다.");
	ActorPartAnimation* pAnimation = animationMap_[animationCode];

	runningAnimation_ = pAnimation;
	runningAnimation_->reflectAnimation(_pRunningAnimation);
}
