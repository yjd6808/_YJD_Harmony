/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:44 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "ActorSprite.h"

#include <SteinsGate/Client/Actor.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
bool ActorSprite::PartData::operator==(const PartData& _other)
{
	return Part == _other.Part && Canvas == _other.Canvas && BoundingBox == _other.BoundingBox;
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorSprite::ActorSprite(
	Actor* _pActor,
	ActorSpriteData* _pActorSpriteData)
	: frameCount_(InvalidValue_v)
	, actor_(_pActor)
	, actorData_(_pActorSpriteData)
	, parts_(_pActorSpriteData->Parts.Size())
	// , m_vPartsCanvas(actorSpriteData->Parts.Size(), nullptr)
	// , m_vParts(actorSpriteData->Parts.Size(), nullptr)
	// , m_vPartsBoundingBox(actorSpriteData->Parts.Size(), nullptr)
	, direction_(SpriteDirection::Right)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorSprite* ActorSprite::create(Actor* _pActor, ActorSpriteData* _pActorSpriteData)
{
	ActorSprite* pSprite = dbg_new ActorSprite(_pActor, _pActorSpriteData);

	if (pSprite && pSprite->init())
	{
		pSprite->autorelease();
		return pSprite;
	}

	JCORE_DELETE_SAFE(pSprite);
	return pSprite;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ActorSprite::init()
{
	if (!Sprite::init())
		return false;

	SGVector<ActorPartSpriteData>& partsData = actorData_->Parts;

	// 바디 파츠 기준으로 전체 프레임수를 얻는다.
	frameCount_ = ImagePackManager::Get()
		->getPack(partsData[0].SgaIndex)
		->getSpriteCount(partsData[0].ImgIndex);

	for (int i = 0; i < partsData.Size(); ++i)
	{
		if (i == 0)
		{
			Core::Contents.PackManager->logTexture("ActorSprite::init()", { partsData[i].SgaIndex, partsData[i].ImgIndex, InvalidValue_v }, LoggerAbstract::eDebug);
		}

		PartData partData = createPart(partsData[i], frameCount_);
		partData.Part->setPartIndex(i);
		parts_.PushBack(partData);
		this->addChild(partData.Canvas, i); // 정렬된 순서대로 ZOrder 반영
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::update(float _dt)
{
	for (int i = 0; i < parts_.Size(); ++i)
	{
		parts_[i].Part->update(_dt);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::runAnimation(int _code)
{
	for (int i = 0; i < parts_.Size(); ++i)
	{
		parts_[i].Part->runAnimation(_code);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::runAnimation(int _code, int _startFrameIndexInAnimation)
{
	for (int i = 0; i < parts_.Size(); ++i)
	{
		parts_[i].Part->runAnimation(_code, _startFrameIndexInAnimation);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	// 무조건 마지막 인덱스 기준으로 해줘야함.
	// 0 ~ 8번 순서대로 업데이트를 진행하는데
	// 만약 0번 인덱스의 onFrameEnd 호출 후 뒤의 애니메이션 상태를 바꿔버리면
	// 해당 프레임때 곧바로 적용되어버리기 1 ~ 7번 프레임은 바뀐 상태로 업데이트를 수행함. 때문에 프레임이 1칸씩 밀릴 수도 있음
	if (_pAnimation->getPartIndex() == parts_.Size() - 1)
		actor_->onFrameBegin(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (_pAnimation->getPartIndex() == parts_.Size() - 1)
		actor_->onFrameEnd(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (_pAnimation->getPartIndex() == parts_.Size() - 1)
		actor_->onAnimationBegin(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (_pAnimation->getPartIndex() == parts_.Size() - 1)
		actor_->onAnimationEnd(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::setSpriteDirection(SpriteDirection_t _direction)
{
	if (_direction == SpriteDirection::Right)
		setForwardDirection();
	else if (_direction == SpriteDirection::Left)
		setBackwardDirection();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::setForwardDirection()
{
	setRotationSkewY(0);
	direction_ = SpriteDirection::Right;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::setBackwardDirection()
{
	setRotationSkewY(180.0f);
	direction_ = SpriteDirection::Left;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::pauseAnimation()
{
	for (int i = 0; i < parts_.Size(); ++i)
	{
		ActorPartAnimation* pAnimation = parts_[i].Part->getRunningAnimation();

		if (pAnimation)
			pAnimation->pause();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::pauseAnimation(float _delay)
{
	for (int i = 0; i < parts_.Size(); ++i)
	{
		ActorPartAnimation* pAnimation = parts_[i].Part->getRunningAnimation();

		if (pAnimation)
			pAnimation->pauseTime(_delay);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::resumeAnimation()
{
	for (int i = 0; i < parts_.Size(); ++i)
	{
		ActorPartAnimation* pAnimation = parts_[i].Part->getRunningAnimation();

		if (pAnimation)
			pAnimation->resume();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartSprite* ActorSprite::getBodyPart()
{
	return parts_[0].Part;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGNode* ActorSprite::getBodyCanvas()
{
	return parts_[0].Part->getCanvas();
}

//////////////////////////////////////////////////////////////////////////////////////////
SGSize ActorSprite::getBodyCanvasSize()
{
	return parts_[0].Part->getCanvas()->getContentSize();
}

//////////////////////////////////////////////////////////////////////////////////////////
SGSize ActorSprite::getBodyPartSize()
{
	return parts_[0].Part->getPartBoundingBox()->getContentSize();
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 ActorSprite::getBodyPartPosition()
{
	return parts_[0].Part->getPosition();
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorSprite::PartData ActorSprite::createPart(const ActorPartSpriteData& _partSpriteData, int _frameCount)
{
	PartData partData;

	// 캔버스 위에 파츠를 그린다.
	partData.Canvas = SGSprite::create();
	partData.Canvas->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	partData.Canvas->setContentSize({ 0, 0 });
	partData.Canvas->setCascadeOpacityEnabled(false);
	partData.Canvas->setOpacity(0);
	partData.BoundingBox = SGDrawNode::create();
	partData.Part = ActorPartSprite::create(
		_frameCount,
		this,
		partData.Canvas,
		partData.BoundingBox,
		_partSpriteData,
		&actorData_->Animations
	);

	// 프로젝틸은 캔버스를 사용하지 않을 거기 땜에
	// 앵커를 0.5, 0.5로 하도록 한다.
	// 캔버스 위에서 그려지는 캐릭터나 몬스터, 기타 오브젝트들은 ZERO로 처리하도록..

	if (actor_->getType() == ActorType::Projectile)
	{
		partData.Part->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		partData.Canvas->addChild(partData.BoundingBox);
	}
	else
	{
		partData.Part->setAnchorPoint(Vec2::ZERO);
		partData.Part->addChild(partData.BoundingBox);
	}

	partData.Canvas->addChild(partData.Part);
	return partData;
}

//////////////////////////////////////////////////////////////////////////////////////////
SpriteDirection_t ActorSprite::getSpriteDirection()
{
	return direction_;
}

//////////////////////////////////////////////////////////////////////////////////////////
int ActorSprite::getRunningAnimationCode()
{
	ActorPartAnimation* pRunningAnimation = parts_[0].Part->getRunningAnimation();
	DebugAssertMsg(pRunningAnimation != nullptr, "실행중인 애니메이션이 없을 수 없습니다.");
	return pRunningAnimation->getAnimationCode();
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartAnimation* ActorSprite::getRunningAnimation() const
{
	ActorPartAnimation* pRunningAnimation = parts_[0].Part->getRunningAnimation();
	DebugAssertMsg(pRunningAnimation != nullptr, "실행중인 애니메이션이 없을 수 없습니다.");
	return pRunningAnimation;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActorSprite::updateSpriteData(ActorSpriteData* _pSpriteData)
{
	actorData_ = _pSpriteData;

	// 다른 부위만 업데이트 해줘야함
	SGVector<ActorPartSpriteData>& partsData = actorData_->Parts;

	// 기존 파츠에서 실행중인 애니메이션 정보를 가져온다.
	// 바디 파츠는 액터 생성중 불변이므로, 바디 파츠 애니메이션 정보 또한 교체될일이 없다.
	ActorPartAnimation* pRunningAnimation = getRunningAnimation();

	SGVector<PartData> removeParts; // 기존 목록중 교체되어야하는 파츠들을 찾는다.
	SGVector<ActorPartSpriteData> newParts; // 신규 목록중 추가되어야하는 파츠들

	// Step 1. 기존 목록중 교체되어야하는 파츠들을 찾는다.
	parts_.Extension().Filter([&partsData](PartData& candidatePart)
		{
			ActorPartSpriteData targetPartData = candidatePart.Part->getPartData();

			bool needToRemove = true;
			for (int i = 0; i < partsData.Size(); ++i)
			{
				// 모두 같으면 동일한 파츠
				if (partsData[i].ImgIndex == targetPartData.ImgIndex &&
					partsData[i].SgaIndex == targetPartData.SgaIndex)
				{
					needToRemove = false;
					break;
				}
			}

			return needToRemove;
		})
		.ForEach([&removeParts](PartData& removePartData)
			{
				removeParts.PushBack(removePartData);
			});

	// Step 2. 신규 목록중 추가되어야하는 파츠들을 찾는다.
	partsData.Extension().Filter([this](ActorPartSpriteData& candidatePart)
		{
			bool needToAdd = true;

			for (int i = 0; i < parts_.Size(); ++i)
			{
				const ActorPartSpriteData& partData = parts_[i].Part->getPartData();

				// 같은 파츠가 이미 기존 파츠 목록에 존재하면 추가해선 안된다.
				if (partData.ImgIndex == candidatePart.ImgIndex &&
					partData.SgaIndex == candidatePart.SgaIndex)
				{
					needToAdd = false;
					break;
				}
			}

			return needToAdd;
		})
		.ForEach([&newParts](ActorPartSpriteData& partData)
			{
				newParts.PushBack(partData);
			});

	// Step 3. 제거된 기존 파츠 목록과 메모리에서 제거한다.
	//         이때 Step4에서 애니메이션으로 등록할 파츠가 제거될 수 있으므로 메모리에서 제거는 하지 않는다.
	removeParts.ForEach([this](PartData& removePart)
		{
			// 캔버스만 제거하면 
			// 캔버스에 붙은 파츠
			// 파츠에 붙은 바운딩박스 알아서 제거됨
			parts_.Remove(removePart);
		});

	// Step 4. 기존 파츠들의 참조 ActorSpriteData를 참조하고 있는 변수들의 데이터를 업데이트 시켜준다.
	for (int i = 0; i < parts_.Size(); ++i)
	{
		parts_[i].Part->updateAnimationReference(&actorData_->Animations);
	}

	// Step 5. 신규 파츠들을 생성한 후 추가한 후 애니메이션 정보를 세팅해준다.
	for (int i = 0; i < newParts.Size(); ++i)
	{
		PartData partData = createPart(newParts[i], frameCount_);
		partData.Part->reflectAnimation(pRunningAnimation);
		parts_.PushBack(partData);
	}

	// Step 6. Z 오더 정렬을 수행한다.
	parts_.Sort([](PartData& lhs, PartData& rhs)
		{
			return lhs.Part->getPartData().ZOrder < rhs.Part->getPartData().ZOrder;
		});

	// Step 7. 변경된 파츠 인덱스 반영
	for (int i = 0; i < parts_.Size(); ++i)
	{
		parts_[i].Part->setPartIndex(i);
	}

	// Step 8. 마지막으로 제거되어야할 대상들을 메모리에서 제거
	removeParts.ForEach([this](PartData& removePart)
		{
			// 파츠에 붙은 바운딩박스 알아서 제거됨
			this->removeChild(removePart.Part, true);
		});
}
