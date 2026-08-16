/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:44 PM
 * =====================
 *
 */

#include "GameCoreHeader.h"
#include "Game/Actor/ActorSprite.h"

#include "sgcl/Game/Actor/Actor.h"

#include "Game/Texture/ImagePackManager.h"

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
bool ActorSprite::PartData::operator==(const PartData& _other)
{
	return pPart_ == _other.pPart_ && canvas_ == _other.canvas_ && boundingBox_ == _other.boundingBox_;
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorSprite::ActorSprite(
	Actor* _pActor,
	ActorSpriteData* _pActorSpriteData)
: frameCount_(InvalidValue_v)
, pActor_(_pActor)
, pActorSpriteData_(_pActorSpriteData)
, parts_(_pActorSpriteData->parts_.Size())
, direction_(SpriteDirection::Right)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorSprite* ActorSprite::Create(Actor* _pActor, ActorSpriteData* _pActorSpriteData)
{
	ActorSprite* pSprite = dbg_new ActorSprite(_pActor, _pActorSpriteData);

	if (pSprite && pSprite->init())
	{
		pSprite->autorelease();
		return pSprite;
	}

	JC_DELETE_SAFE(pSprite);
	return pSprite;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ActorSprite::init()
{
	if (!Sprite::init())
		return false;

	jc::Vector<ActorPartSpriteData>& partsDataVector = pActorSpriteData_->parts_;

	// 바디 파츠 기준으로 전체 프레임수를 얻는다.
	frameCount_ = g_cImagePackMgr.GetPack(partsDataVector[0].sgaIndex_)->GetSpriteCount(partsDataVector[0].imgIndex_);

	for (int i = 0; i < partsDataVector.Size(); ++i)
	{
		if (i == 0)
		{
			g_cImagePackMgr.LogTexture("ActorSprite::init()",
			                                       {
				                                       partsDataVector[i].sgaIndex_, partsDataVector[i].imgIndex_,
				                                       InvalidValue_v
			                                       }, LoggerAbstract::eDebug);
		}

		PartData partData = CreatePart(partsDataVector[i], frameCount_);
		partData.pPart_->SetPartIndex(i);
		parts_.PushBack(partData);
		this->addChild(partData.canvas_, i); // 정렬된 순서대로 ZOrder 반영
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::update(float _deltaTime)
{
	for (int i = 0; i < parts_.Size(); ++i)
	{
		parts_[i].pPart_->update(_deltaTime);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::RunAnimation(int _code)
{
	for (int i = 0; i < parts_.Size(); ++i)
	{
		parts_[i].pPart_->RunAnimation(_code);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::RunAnimation(int _code, int _startFrameIndexInAnimation)
{
	for (int i = 0; i < parts_.Size(); ++i)
	{
		parts_[i].pPart_->RunAnimation(_code, _startFrameIndexInAnimation);
	}
}

// 무조건 마지막 인덱스 기준으로 해줘야함.
// 0 ~ 8번 순서대로 업데이트를 진행하는데
// 만약 0번 인덱스의 onFrameEnd 호출 후 뒤의 애니메이션 상태를 바꿔버리면
// 해당 프레임때 곧바로 적용되어버리기 1 ~ 7번 프레임은 바뀐 상태로 업데이트를 수행함. 때문에 프레임이 1칸씩 밀릴 수도 있음
//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (_pAnimation->GetPartIndex() == parts_.Size() - 1)
		pActor_->OnFrameBegin(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (_pAnimation->GetPartIndex() == parts_.Size() - 1)
		pActor_->OnFrameEnd(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (_pAnimation->GetPartIndex() == parts_.Size() - 1)
		pActor_->OnAnimationBegin(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (_pAnimation->GetPartIndex() == parts_.Size() - 1)
		pActor_->OnAnimationEnd(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::SetSpriteDirection(SpriteDirection_t _direction)
{
	if (_direction == SpriteDirection::Right)
		SetForwardDirection();
	else if (_direction == SpriteDirection::Left)
		SetBackwardDirection();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::SetForwardDirection()
{
	setRotationSkewY(0);
	direction_ = SpriteDirection::Right;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::SetBackwardDirection()
{
	setRotationSkewY(180.0f);
	direction_ = SpriteDirection::Left;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::PauseAnimation()
{
	for (int i = 0; i < parts_.Size(); ++i)
	{
		ActorPartAnimation* pAnimation = parts_[i].pPart_->GetRunningAnimation();

		if (pAnimation)
			pAnimation->Pause();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::PauseAnimation(float _delay)
{
	for (int i = 0; i < parts_.Size(); ++i)
	{
		ActorPartAnimation* pAnimation = parts_[i].pPart_->GetRunningAnimation();

		if (pAnimation)
			pAnimation->PauseTime(_delay);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::ResumeAnimation()
{
	for (int i = 0; i < parts_.Size(); ++i)
	{
		ActorPartAnimation* pAnimation = parts_[i].pPart_->GetRunningAnimation();

		if (pAnimation)
			pAnimation->Resume();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartSprite* ActorSprite::GetBodyPart()
{
	return parts_[0].pPart_;
}

//////////////////////////////////////////////////////////////////////////////////////////
cc::Node* ActorSprite::GetBodyCanvas()
{
	return parts_[0].pPart_->GetCanvas();
}

//////////////////////////////////////////////////////////////////////////////////////////
cc::size ActorSprite::GetBodyCanvasSize()
{
	return parts_[0].pPart_->GetCanvas()->getContentSize();
}

//////////////////////////////////////////////////////////////////////////////////////////
cc::size ActorSprite::GetBodyPartSize()
{
	return parts_[0].pPart_->GetPartBoundingBox()->getContentSize();
}

//////////////////////////////////////////////////////////////////////////////////////////
cc::vec2 ActorSprite::GetBodyPartPosition()
{
	return parts_[0].pPart_->getPosition();
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorSprite::PartData ActorSprite::CreatePart(const ActorPartSpriteData& _partSpriteData, int _frameCount)
{
	PartData partData;

	// 캔버스 위에 파츠를 그린다.
	partData.canvas_ = cc::Sprite::create();
	partData.canvas_->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	partData.canvas_->setContentSize({ 0, 0 });
	partData.canvas_->setCascadeOpacityEnabled(false);
	partData.canvas_->setOpacity(0);
	partData.boundingBox_ = cc::DrawNode::create();
	partData.pPart_ = ActorPartSprite::Create(
		_frameCount,
		this,
		partData.canvas_,
		partData.boundingBox_,
		_partSpriteData,
		&pActorSpriteData_->animations_
	);

	// 프로젝틸은 캔버스를 사용하지 않을 거기 땜에
	// 앵커를 0.5, 0.5로 하도록 한다.
	// 캔버스 위에서 그려지는 캐릭터나 몬스터, 기타 오브젝트들은 ZERO로 처리하도록..

	if (pActor_->GetType() == ActorType::Projectile)
	{
		partData.pPart_->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		partData.canvas_->addChild(partData.boundingBox_);
	}
	else
	{
		partData.pPart_->setAnchorPoint(Vec2::ZERO);
		partData.pPart_->addChild(partData.boundingBox_);
	}

	partData.canvas_->addChild(partData.pPart_);
	return partData;
}

//////////////////////////////////////////////////////////////////////////////////////////
SpriteDirection_t ActorSprite::GetSpriteDirection()
{
	return direction_;
}

//////////////////////////////////////////////////////////////////////////////////////////
int ActorSprite::GetRunningAnimationCode()
{
	ActorPartAnimation* pRunningAnimation = parts_[0].pPart_->GetRunningAnimation();
	jc_assert_msg(pRunningAnimation != nullptr, "실행중인 애니메이션이 없을 수 없습니다.");
	return pRunningAnimation->GetAnimationCode();
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartAnimation* ActorSprite::GetRunningAnimation() const
{
	ActorPartAnimation* pRunningAnimation = parts_[0].pPart_->GetRunningAnimation();
	jc_assert_msg(pRunningAnimation != nullptr, "실행중인 애니메이션이 없을 수 없습니다.");
	return pRunningAnimation;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::UpdateSpriteData(ActorSpriteData* _pSpriteData)
{
	pActorSpriteData_ = _pSpriteData;

	// 다른 부위만 업데이트 해줘야함
	jc::Vector<ActorPartSpriteData>& partsDataVector = pActorSpriteData_->parts_;

	// 기존 파츠에서 실행중인 애니메이션 정보를 가져온다.
	// 바디 파츠는 액터 생성중 불변이므로, 바디 파츠 애니메이션 정보 또한 교체될일이 없다.
	ActorPartAnimation* pRunningAnimation = GetRunningAnimation();

	jc::Vector<PartData> removeParts; // 기존 목록중 교체되어야하는 파츠들을 찾는다.
	jc::Vector<ActorPartSpriteData> newParts; // 신규 목록중 추가되어야하는 파츠들

	// Step 1. 기존 목록중 교체되어야하는 파츠들을 찾는다.
	parts_.ForEach([&partsDataVector, &removeParts](PartData& _candidatePart)
	{
		ActorPartSpriteData targetPartData = _candidatePart.pPart_->GetPartData();

		bool needToRemove = true;
		for (int i = 0; i < partsDataVector.Size(); ++i)
		{
			// 모두 같으면 동일한 파츠
			if (partsDataVector[i].imgIndex_ == targetPartData.imgIndex_ &&
				partsDataVector[i].sgaIndex_ == targetPartData.sgaIndex_)
			{
				needToRemove = false;
				break;
			}
		}

		if (needToRemove)
		{
			removeParts.PushBack(_candidatePart);
		}
	});

	// Step 2. 신규 목록중 추가되어야하는 파츠들을 찾는다.
	partsDataVector.ForEach([this, &newParts](ActorPartSpriteData& _candidatePart)
	{
		bool needToAdd = true;

		for (int i = 0; i < parts_.Size(); ++i)
		{
			const ActorPartSpriteData& partData = parts_[i].pPart_->GetPartData();

			// 같은 파츠가 이미 기존 파츠 목록에 존재하면 추가해선 안된다.
			if (partData.imgIndex_ == _candidatePart.imgIndex_ &&
				partData.sgaIndex_ == _candidatePart.sgaIndex_)
			{
				needToAdd = false;
				break;
			}
		}

		if (needToAdd)
		{
			newParts.PushBack(_candidatePart);
		}
	});

	// Step 3. 제거된 기존 파츠 목록과 메모리에서 제거한다.
	//         이때 Step4에서 애니메이션으로 등록할 파츠가 제거될 수 있으므로 메모리에서 제거는 하지 않는다.
	removeParts.ForEach([this](PartData& _removePart)
	{
		// 캔버스만 제거하면 
		// 캔버스에 붙은 파츠
		// 파츠에 붙은 바운딩박스 알아서 제거됨
		parts_.Remove(_removePart);
	});

	// Step 4. 기존 파츠들의 참조 ActorSpriteData를 참조하고 있는 변수들의 데이터를 업데이트 시켜준다.
	for (int i = 0; i < parts_.Size(); ++i)
	{
		parts_[i].pPart_->UpdateAnimationReference(&pActorSpriteData_->animations_);
	}

	// Step 5. 신규 파츠들을 생성한 후 추가한 후 애니메이션 정보를 세팅해준다.
	for (int i = 0; i < newParts.Size(); ++i)
	{
		PartData partData = CreatePart(newParts[i], frameCount_);
		partData.pPart_->ReflectAnimation(pRunningAnimation);
		parts_.PushBack(partData);
	}

	// Step 6. Z 오더 정렬을 수행한다.
	parts_.Sort([](PartData& _lhs, PartData& _rhs)
	{
		return _lhs.pPart_->GetPartData().zOrder_ < _rhs.pPart_->GetPartData().zOrder_;
	});

	// Step 7. 변경된 파츠 인덱스 반영
	for (int i = 0; i < parts_.Size(); ++i)
	{
		parts_[i].pPart_->SetPartIndex(i);
	}

	// Step 8. 마지막으로 제거되어야할 대상들을 메모리에서 제거
	removeParts.ForEach([this](PartData& _removePart)
	{
		// 파츠에 붙은 바운딩박스 알아서 제거됨
		this->removeChild(_removePart.pPart_, true);
	});
}
