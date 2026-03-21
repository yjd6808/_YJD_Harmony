/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 3:47:17 PM
 * =====================
 *
 */

#include "Effect.h"

#include "sgcl/ImagePackManager.h"
#include "sgcl/ActorSprite.h"
#include "sg/_Util/DescLoaderMgr.h"
#include "sgcl/HostPlayer.h"
#include "sgcl/_Core/AppConfig_Props.h"


USING_NS_JC;
USING_NS_CC;

Effect::Effect(EffectInfo* _pBaseInfo)
: pBaseInfo_(_pBaseInfo)
{
}

Effect::~Effect()
{
}

Effect* Effect::Create(EffectInfo* _pBaseInfo)
{
	Effect* pEffect = dbg_new Effect(_pBaseInfo);
	pEffect->Initialize();
	pEffect->autorelease();
	return pEffect;
}

void Effect::Initialize()
{
	InitActorSprite();
	InitVariables();
	InitListeners();
	InitComponents();
}

bool Effect::InitVariables()
{
	Actor::InitVariables();

	return true;
}

// 프로젝틸은 파츠, 애니메이션 다 1개씩임
void Effect::InitActorSprite()
{
	pActorSprite_ = ActorSprite::Create(this, pBaseInfo_->pSpriteData_);
	pActorSprite_->setAnchorPoint(Vec2::ZERO);
	this->addChild(pActorSprite_);
}

void Effect::InitListeners()
{
}

void Effect::InitComponents()
{
}

void Effect::update(float _delta)
{
	Actor::update(_delta);

	if (g_cAppProps.GetBool(CONF_PROPS_DRAW_EFFECT))
	{
		pActorSprite_->GetParts().ForEach([](ActorSprite::PartData& part) { part.pPart_->setOpacity(255); });
	}
	else
	{
		pActorSprite_->GetParts().ForEach([](ActorSprite::PartData& part) { part.pPart_->setOpacity(0); });
	}
}

void Effect::OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
}

void Effect::OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
}

void Effect::OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
}

void Effect::OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	// 일단 애니메이션 모두 실행 후 소멸된다고 가정
	// 추후 이펙트 추가된다면 프로젝틸 리스너처럼 이펙트 리스너로 기능 개별 구현 필요
	CleanUpAtNextFrame();
}
