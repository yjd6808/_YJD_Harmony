/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:44 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGActorSprite.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGActor.h>

USING_NS_CC;
USING_NS_JC;

SGActorSprite::SGActorSprite(
	SGActor* actor, 
	const SGActorSpriteDataPtr& actorData)
	: m_iFrameCount(InvalidValue_v)
	, m_pActor(actor)
	, m_spActorData(actorData)
	, m_vParts(actorData->Parts.Size())
	// , m_vPartsCanvas(actorData->Parts.Size(), nullptr)
	// , m_vParts(actorData->Parts.Size(), nullptr)
	// , m_vPartsBoundingBox(actorData->Parts.Size(), nullptr)
	, m_eDirection(SpriteDirection::Right)

{
}

SGActorSprite* SGActorSprite::create(
	SGActor* actor, const 
	SGActorSpriteDataPtr& actorData)
{
	SGActorSprite* pSprite = dbg_new SGActorSprite(actor, actorData);

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

	SGVector<ActorPartSpriteData>& vPartsData = m_spActorData->Parts;
	vPartsData.Sort([](ActorPartSpriteData& lhs, ActorPartSpriteData& rhs) { return lhs.ZOrder < rhs.ZOrder; });

	// 바디 파츠 기준으로 전체 프레임수를 얻는다.
	m_iFrameCount = SGImagePackManager::get()
		->getPack(vPartsData[0].SgaIndex)
		->getSpriteCount(vPartsData[0].ImgIndex);

	for (int i = 0; i < vPartsData.Size(); ++i) {

		if (i == 0) {
			SGImagePack* pPack = CorePackManager_v->getPack(vPartsData[i].SgaIndex);
			SGString szPackName = pPack->getFileName();
			SGString& szImgName = pPack->getImgName(vPartsData[i].ImgIndex);

			_LogDebug_("액터 로딩: %s || sga: %s || img: %s || z: %d", ActorType::Name[m_pActor->getType()], szPackName.Source(), szImgName.Source(), vPartsData[i].ZOrder);
		}

		PartData partData = createPart(vPartsData[i], m_iFrameCount);
		partData.Part->setPartIndex(i);
		m_vParts.PushBack(partData);
		this->addChild(partData.Canvas, i);	// 정렬된 순서대로 ZOrder 반영
	}

	return true;
}

void SGActorSprite::update(float dt) {
	for (int i = 0; i < m_vParts.Size(); ++i) {
		m_vParts[i].Part->update(dt);
	}
}

void SGActorSprite::runAnimation(int code) {
	for (int i = 0; i < m_vParts.Size(); ++i) {
		m_vParts[i].Part->runAnimation(code);
	}
}

void SGActorSprite::runAnimation(int code, int startFrameIndexInAnimation) {
	for (int i = 0; i < m_vParts.Size(); ++i) {
		m_vParts[i].Part->runAnimation(code, startFrameIndexInAnimation);
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

void SGActorSprite::pauseAnimation() {
	for (int i = 0; i < m_vParts.Size(); ++i) {
		SGActorPartAnimation* pAnimation = m_vParts[i].Part->getRunningAnimation();

		if (pAnimation)
			pAnimation->pause();
	}
}

void SGActorSprite::pauseAnimation(float delay) {
	for (int i = 0; i < m_vParts.Size(); ++i) {
		SGActorPartAnimation* pAnimation = m_vParts[i].Part->getRunningAnimation();

		if (pAnimation)
			pAnimation->pauseTime(delay);
	}
}

void SGActorSprite::resumeAnimation() {
	for (int i = 0; i < m_vParts.Size(); ++i) {
		SGActorPartAnimation* pAnimation = m_vParts[i].Part->getRunningAnimation();

		if (pAnimation)
			pAnimation->resume();
	}
}

SGActorPartSprite* SGActorSprite::getBodyPart() {
	return m_vParts[0].Part;
}

SGNode* SGActorSprite::getBodyCanvas() {
	return m_vParts[0].Part->getCanvas();
}

SGSize SGActorSprite::getBodyCanvasSize() {
	return m_vParts[0].Part->getCanvas()->getContentSize();
}

SGSize SGActorSprite::getBodyPartSize() {
	return m_vParts[0].Part->getPartBoundingBox()->getContentSize();
}

SGVec2 SGActorSprite::getBodyPartPosition() {
	return m_vParts[0].Part->getPosition();
}

SGActorSprite::PartData SGActorSprite::createPart(const ActorPartSpriteData& partSpriteData, int frameCount) {
	PartData partData;

	// 캔버스 위에 파츠를 그린다.
	partData.Canvas = SGSprite::create();
	partData.Canvas->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	partData.Canvas->setContentSize({ 0, 0 });
	partData.Canvas->setCascadeOpacityEnabled(false);
	partData.Canvas->setOpacity(0);
	partData.BoundingBox = SGDrawNode::create();
	partData.Part = SGActorPartSprite::create(
		frameCount,
		this,
		partData.Canvas,
		partData.BoundingBox,
		partSpriteData,
		m_spActorData->Animations
	);

	// 프로젝틸은 캔버스를 사용하지 않을 거기 땜에
	// 앵커를 0.5, 0.5로 하도록 한다.
	// 캔버스 위에서 그려지는 캐릭터나 몬스터, 기타 오브젝트들은 ZERO로 처리하도록..
	if (m_pActor->getType() == ActorType::Projectile) {
		partData.Part->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		partData.Canvas->addChild(partData.BoundingBox);
	}
	else {
		partData.Part->setAnchorPoint(Vec2::ZERO);
		partData.Part->addChild(partData.BoundingBox);
	}
	partData.Canvas->addChild(partData.Part);
	return partData;
}

SpriteDirection_t SGActorSprite::getSpriteDirection() {
	return m_eDirection;
}

int SGActorSprite::getRunningAnimationCode() {
	SGActorPartAnimation* pRunningAnimation = m_vParts[0].Part->getRunningAnimation();
	DebugAssertMsg(pRunningAnimation != nullptr, "실행중인 애니메이션이 없을 수 없습니다.");
	return pRunningAnimation->getAnimationCode();
}

SGActorPartAnimation* SGActorSprite::getRunningAnimation() {
	SGActorPartAnimation* pRunningAnimation = m_vParts[0].Part->getRunningAnimation();
	DebugAssertMsg(pRunningAnimation != nullptr, "실행중인 애니메이션이 없을 수 없습니다.");
	return pRunningAnimation;
}

void SGActorSprite::updateSpriteData(const SGActorSpriteDataPtr& spriteData) {

	// 다른 부위만 업데이트 해줘야함
	SGVector<ActorPartSpriteData>& vPartsData = m_spActorData->Parts;

	// 기존 파츠에서 실행중인 애니메이션 정보를 가져온다.
	SGActorPartAnimation* pRunningAnimation = getRunningAnimation();

	
	SGVector<PartData> vRemoveParts;				// 기존 목록중 교체되어야하는 파츠들을 찾는다.
	SGVector<ActorPartSpriteData> vNewParts;		// 신규 목록중 추가되어야하는 파츠들

	// Step 1. 기존 목록중 교체되어야하는 파츠들을 찾는다.
	m_vParts.Extension().Filter([&vPartsData](PartData& candidatePart) {
		ActorPartSpriteData pTargetPartData = candidatePart.Part->getPartData();

		bool bNeedToRemove = true;
		for (int i = 0; i < vPartsData.Size(); ++i) {

			// 모두 같으면 동일한 파츠
			if (vPartsData[i].ImgIndex == pTargetPartData.ImgIndex &&
				vPartsData[i].SgaIndex == pTargetPartData.SgaIndex) {
				bNeedToRemove = false;
				break;
			}

		}

		return bNeedToRemove;

	}).ForEach([&vRemoveParts](PartData& removePartData) {
		vRemoveParts.PushBack(removePartData);
	});


	// Step 2. 신규 목록중 추가되어야하는 파츠들을 찾는다.
	vPartsData.Extension().Filter([this](ActorPartSpriteData& candidatePart) {
		bool bNeedToAdd = false;

		for (int i = 0; i < m_vParts.Size(); ++i) {
			const ActorPartSpriteData& partData = m_vParts[i].Part->getPartData();

			// 하나라도 다르면 다른 파츠
			if (partData.ImgIndex != candidatePart.ImgIndex ||
				partData.SgaIndex != candidatePart.SgaIndex) {
				bNeedToAdd = true;
				break;
			}
		}

		return bNeedToAdd;
	}).ForEach([&vNewParts](ActorPartSpriteData& partData) {
		vNewParts.PushBack(partData);
	});

	// Step 3. 제거된 기존 파츠 목록과 메모리에서 제거한다.
	//         이때 Step4에서 애니메이션으로 등록할 파츠가 제거될 수 있으므로 메모리에서 제거는 하지 않는다.
	vRemoveParts.Extension().ForEach([this](PartData& removePart) {

		// 캔버스만 제거하면 
		// 캔버스에 붙은 파츠
		// 파츠에 붙은 바운딩박스 알아서 제거됨
		m_vParts.Remove(removePart);
	});


	// Step 4. 신규 파츠들을 생성한 후 추가한 후 애니메이션 정보를 세팅해준다.
	for (int i = 0; i < vNewParts.Size(); ++i) {
		 PartData partData = createPart(vNewParts[i], m_iFrameCount);
		 partData.Part->reflectAnimation(pRunningAnimation);
		 m_vParts.PushBack(partData);
	}

	// Step 5. Z 오더 정렬을 수행한다.
	m_vParts.Sort([](PartData& lhs, PartData& rhs) {
		return lhs.Part->getPartData().ZOrder < rhs.Part->getPartData().ZOrder;
	});

	// Step 6. 변경된 파츠 인덱스 반영
	for (int i = 0; i < m_vParts.Size(); ++i) {
		m_vParts[i].Part->setPartIndex(i);
	}

	// Step 7. 마지막으로 제거되어야할 대상들을 메모리에서 제거
	vRemoveParts.Extension().ForEach([this](PartData& removePart) {
		// 파츠에 붙은 바운딩박스 알아서 제거됨
		this->removeChild(removePart.Part, true);
	});
}
