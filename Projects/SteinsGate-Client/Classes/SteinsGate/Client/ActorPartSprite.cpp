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
	int _frameCount,
	ActorSprite* _pActor,
	SGNode* _pCanvas,
	SGDrawNode* _pBoundingBox,
	const ActorPartSpriteData& _partData,
	SGVector<AnimationInfo>* _pAnimations
)
{
	ActorPartSprite* pPartSprite = dbg_new ActorPartSprite(_frameCount, _pActor, _pCanvas, _pBoundingBox, _partData,
	                                                       _pAnimations);

	if (pPartSprite && pPartSprite->init())
	{
		pPartSprite->autorelease();
		return pPartSprite;
	}

	JCORE_DELETE_SAFE(pPartSprite);
	return nullptr;
}

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

ActorPartSprite::~ActorPartSprite()
{
	animationMap_.ForEachValue([](ActorPartAnimation* _pAnimation) { CC_SAFE_RELEASE(_pAnimation); });

	for (int i = 0; i < frames_.Size(); ++i)
	{
		CC_SAFE_RELEASE(frames_[i]);
	}
}

bool ActorPartSprite::init()
{
	if (!Sprite::init())
	{
		return false;
	}

	initFrames();
	initAnimations();

	return true;
}

void ActorPartSprite::initFrames()
{
	ImagePack* pImgPack = ImagePackManager::Get()->getPack(partData_.SgaIndex);

	for (int i = 0; i < frames_.Size(); ++i)
	{
		frames_[i] = pImgPack->createFrameTexture(partData_.ImgIndex, i);

		if (frames_[i] == nullptr)
		{
			frames_[i] = Global::Get()->getDefaultFrameTexture();
		}

		frames_[i]->retain();
	}
}

void ActorPartSprite::initAnimations()
{
	for (int i = 0; i < animationInfoList_->Size(); ++i)
	{
		AnimationInfo& animationInfo = animationInfoList_->At(i);
		ActorPartAnimation* pPartAnimation = ActorPartAnimation::create(this, &animationInfo, frames_);
		pPartAnimation->constructFrames(partData_.SgaIndex, partData_.ImgIndex);
		pPartAnimation->retain();
		animationMap_.Insert(animationInfo.code_, pPartAnimation);
		animationList_.PushBack(pPartAnimation);
	}
}

void ActorPartSprite::update(float _dt)
{
	if (runningAnimation_)
	{
		runningAnimation_->update(_dt);
	}

	updateBoundingBoxVisibleState();
}

void ActorPartSprite::updateBoundingBoxVisibleState()
{
	if (Global::Get()->DrawBodyBoundingBox)
	{
		boundingBox_->setOpacity(255);
	}
	else
	{
		boundingBox_->setOpacity(0);
	}
}

void ActorPartSprite::updateAnimationReference(SGVector<AnimationInfo>* _pAnimations)
{
	animationInfoList_ = _pAnimations;
	SGVector<ActorPartAnimation*> animationList(animationList_.Size());

	for (int i = 0; i < animationInfoList_->Size(); ++i)
	{
		AnimationInfo& animationInfo = animationInfoList_->At(i);

		if (!animationMap_.Exist(animationInfo.code_))
		{
			DebugAssertMsg(false, "%s", animationInfo.Name.Source());
			return;
		}

		ActorPartAnimation* pAnimation = animationMap_[animationInfo.code_];
		pAnimation->setAnimationInfo(&animationInfo);
		animationList.PushBack(pAnimation);
	}

	DebugAssert(animationList_.Size() == animationList.Size());
	animationList_ = animationList;
}

void ActorPartSprite::runAnimation(int _code)
{
	runAnimation(_code, 0);
}

void ActorPartSprite::runAnimation(int _code, int _startFrameIndexInAnimation)
{
	DebugAssertMsg(animationMap_.Exist(_code), "");
	ActorPartAnimation* pNextAnimation = animationMap_[_code];

	if (runningAnimation_ == pNextAnimation && _startFrameIndexInAnimation == 0)
	{
		runningAnimation_->setLoopSequence();
		return;
	}

	runningAnimation_ = pNextAnimation;
	runningAnimation_->run(_startFrameIndexInAnimation);

	updateBoundingBoxVisibleState();
}

ActorType_t ActorPartSprite::getActorType()
{
	return getActorSprite()->getActor()->getType();
}

void ActorPartSprite::onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	actorSprite_->onAnimationBegin(_pAnimation, _pTexture);
}

void ActorPartSprite::onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	actorSprite_->onAnimationEnd(_pAnimation, _pTexture);
}

void ActorPartSprite::onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	const SgaSpriteRect& rect = _pTexture->getFullRect();
	const int frameIndex = _pTexture->getFrameIndex();
	const int frameIndexInAnimation = _pAnimation->getFrameIndexInAnimation();

	float frameWidth = _pTexture->getFrameWidthF();
	float frameHeight = _pTexture->getFrameHeightF();

	float width = static_cast<float>(rect.width_);
	float height = static_cast<float>(rect.height_);
	float adjustX = static_cast<float>(rect.x_);
	float adjustY = frameHeight - static_cast<float>(rect.y_) - height;

	bool projectile = getActorType() == ActorType::Projectile;

	if (!projectile)
	{
		canvas_->setPosition(-frameWidth / 2.0f, -frameHeight / 2.0f);
		canvas_->setContentSize(frameWidth, frameHeight);

		setContentSize({ width, height });
		setPosition(adjustX, adjustY);
	}

	if (partIndex_ == 0)
	{
		SGRect boundingBox = getBoundingBox();

		RectPoly poly = projectile
			                ? RectPoly::createFromCenter({ 0, 0 }, boundingBox.size)
			                : RectPoly::createFromLeftBottom({ 0, 0 }, boundingBox.size);

		boundingBox_->clear();
		boundingBox_->setContentSize({ boundingBox.size });
		boundingBox_->drawPolygon(poly.source(), 4, {}, 1, Color4F::WHITE);
	}

	actorSprite_->onFrameBegin(_pAnimation, _pTexture);
}

void ActorPartSprite::onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	actorSprite_->onFrameEnd(_pAnimation, _pTexture);
}

void ActorPartSprite::onCustomFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
}

void ActorPartSprite::onCustomFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
}

void ActorPartSprite::reflectAnimation(ActorPartAnimation* _pRunningAnimation)
{
	int animationCode = _pRunningAnimation->getAnimationCode();
	DebugAssertMsg(animationMap_.Exist(animationCode), "");
	ActorPartAnimation* pAnimation = animationMap_[animationCode];

	runningAnimation_ = pAnimation;
	runningAnimation_->reflectAnimation(_pRunningAnimation);
}
