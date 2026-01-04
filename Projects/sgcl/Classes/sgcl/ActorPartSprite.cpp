#include "Core.h"
#include "ActorPartSprite.h"

#include <sgcl/ActorSprite.h>
#include <sgcl/ImagePackManager.h>
#include <sgcl/Global.h>

#include "Actor.h"

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartSprite* ActorPartSprite::Create(
	int _frameCount,
	ActorSprite* _pActor,
	cc::Node* _pCanvas,
	cc::DrawNode* _pBoundingBox,
	const ActorPartSpriteData& _partData,
	jc::Vector<AnimationInfo>* _pAnimations
)
{
	ActorPartSprite* pPartSprite = dbg_new ActorPartSprite(_frameCount, _pActor, _pCanvas, _pBoundingBox, _partData,
	                                                       _pAnimations);

	if (pPartSprite && pPartSprite->init())
	{
		pPartSprite->autorelease();
		return pPartSprite;
	}

	JC_DELETE_SAFE(pPartSprite);
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartSprite::ActorPartSprite(
	int _frameCount,
	ActorSprite* _pActor,
	cc::Node* _pCanvas,
	cc::DrawNode* _pBoundingBox,
	const ActorPartSpriteData& _partData,
	jc::Vector<AnimationInfo>* _pAnimations)
: partIndex_(0)
, pActorSprite_(_pActor)
, partData_(_partData)
, pAnimationInfoList_(_pAnimations)
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
	{
		return false;
	}

	InitFrames();
	InitAnimations();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::InitFrames()
{
	ImagePack* pImgPack = ImagePackManager::Get()->GetPack(partData_.sgaIndex_);

	for (int i = 0; i < frames_.Size(); ++i)
	{
		frames_[i] = pImgPack->CreateFrameTexture(partData_.imgIndex_, i);

		if (frames_[i] == nullptr)
		{
			frames_[i] = Global::Get()->getDefaultFrameTexture();
		}

		frames_[i]->retain();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::InitAnimations()
{
	for (int i = 0; i < pAnimationInfoList_->Size(); ++i)
	{
		AnimationInfo& animationInfo = pAnimationInfoList_->At(i);
		ActorPartAnimation* pPartAnimation = ActorPartAnimation::Create(this, &animationInfo, frames_);
		pPartAnimation->ConstructFrames(partData_.sgaIndex_, partData_.imgIndex_);
		pPartAnimation->retain();
		animationMap_.Insert(animationInfo.code_, pPartAnimation);
		animationList_.PushBack(pPartAnimation);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::update(float _dt)
{
	if (runningAnimation_)
	{
		runningAnimation_->Update(_dt);
	}

	UpdateBoundingBoxVisibleState();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::UpdateBoundingBoxVisibleState()
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

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::UpdateAnimationReference(jc::Vector<AnimationInfo>* _pAnimations)
{
	pAnimationInfoList_ = _pAnimations;
	jc::Vector<ActorPartAnimation*> animationList(animationList_.Size());

	for (int i = 0; i < pAnimationInfoList_->Size(); ++i)
	{
		AnimationInfo& animationInfo = pAnimationInfoList_->At(i);

		if (!animationMap_.Exist(animationInfo.code_))
		{
			jc_assert_msg(false, "%s", animationInfo.name_.Source());
			return;
		}

		ActorPartAnimation* pAnimation = animationMap_[animationInfo.code_];
		pAnimation->SetAnimationInfo(&animationInfo);
		animationList.PushBack(pAnimation);
	}

	jc_assert(animationList_.Size() == animationList.Size());
	animationList_ = animationList;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::RunAnimation(int _code)
{
	RunAnimation(_code, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::RunAnimation(int _code, int _startFrameIndexInAnimation)
{
	jc_assert_msg(animationMap_.Exist(_code), "");
	ActorPartAnimation* pNextAnimation = animationMap_[_code];

	if (runningAnimation_ == pNextAnimation && _startFrameIndexInAnimation == 0)
	{
		runningAnimation_->SetLoopSequence();
		return;
	}

	runningAnimation_ = pNextAnimation;
	runningAnimation_->Run(_startFrameIndexInAnimation);

	UpdateBoundingBoxVisibleState();
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorType_t ActorPartSprite::GetActorType()
{
	return GetActorSprite()->GetActor()->GetType();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	pActorSprite_->OnAnimationBegin(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	pActorSprite_->OnAnimationEnd(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	const SgaSpriteRect& rect = _pTexture->GetFullRect();
	const int frameIndex = _pTexture->GetFrameIndex();
	const int frameIndexInAnimation = _pAnimation->GetFrameIndexInAnimation();

	float frameWidth = _pTexture->GetFrameWidthF();
	float frameHeight = _pTexture->GetFrameHeightF();

	float width = static_cast<float>(rect.width_);
	float height = static_cast<float>(rect.height_);
	float adjustX = static_cast<float>(rect.x_);
	float adjustY = frameHeight - static_cast<float>(rect.y_) - height;

	bool projectile = GetActorType() == ActorType::Projectile;

	if (!projectile)
	{
		canvas_->setPosition(-frameWidth / 2.0f, -frameHeight / 2.0f);
		canvas_->setContentSize(frameWidth, frameHeight);

		setContentSize({ width, height });
		setPosition(adjustX, adjustY);
	}

	if (partIndex_ == 0)
	{
		cc::rect boundingBox = getBoundingBox();

		RectPoly poly = projectile
			                ? RectPoly::createFromCenter({ 0, 0 }, boundingBox.size)
			                : RectPoly::createFromLeftBottom({ 0, 0 }, boundingBox.size);

		boundingBox_->clear();
		boundingBox_->setContentSize({ boundingBox.size });
		boundingBox_->drawPolygon(poly.source(), 4, {}, 1, Color4F::WHITE);
	}

	pActorSprite_->OnFrameBegin(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	pActorSprite_->OnFrameEnd(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::OnCustomFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::OnCustomFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorPartSprite::ReflectAnimation(ActorPartAnimation* _pRunningAnimation)
{
	int animationCode = _pRunningAnimation->GetAnimationCode();
	jc_assert_msg(animationMap_.Exist(animationCode), "");
	ActorPartAnimation* pAnimation = animationMap_[animationCode];

	runningAnimation_ = pAnimation;
	runningAnimation_->ReflectAnimation(_pRunningAnimation);
}
