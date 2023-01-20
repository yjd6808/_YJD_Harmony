/*
 * 작성자: 윤정도
 * 생성일: 1/15/2023 9:38:47 PM
 * =====================
 *
 */

#include "Collider.h"

#include <SteinsGate/Common/Engine/RectPoly.h>
#include <SteinsGate/Research/BulletTarget.h>
#include <SteinsGate/Research/Config.h>
#include <SteinsGate/Research/World.h>


using namespace cocos2d;

Collider::Collider(const int type, const int id) :
	m_iColliderType(type),
	m_iColliderId(id)
{
}

Collider* Collider::create(const int type, const int id) {
	auto p = new Collider(type, id);
	p->autorelease();
	p->init();
	return p;
}

bool Collider::init() {

	//info Loader::GetInfo(m_iColliderType, )m_iColliderId)
	//m_ThicknessBoxSize = info.boxSize
	//m_fThicknessRelativeY = info.relativeY;

	if (m_iColliderType == int(ColliderType::Character)) {
		m_ThicknessBoxSize = Size{ ThicknessBoxWidth_v , ThicknessBoxHeight_v };
		m_fThicknessRelativeY = ThicknessRelativeY_v;
	} else if (m_iColliderType == int(ColliderType::Obstable)) {
		m_ThicknessBoxSize = Size{ 80.0f, 30.0f };
		m_fThicknessRelativeY = -50.0f;
		m_pTarget = ColliderTarget::create();
		this->addChild(m_pTarget);
	} else if (m_iColliderType == int(ColliderType::Projectile)) {
		m_ThicknessBoxSize = Size{ 30.0f, 30.0f };

		if (m_iColliderId == 0)
			m_fThicknessRelativeY = -110.0f;
		else if (m_iColliderId == 1)
			m_fThicknessRelativeY = -110.0f;
		else if (m_iColliderId == 2)
			m_fThicknessRelativeY = -70.0f;
		else if (m_iColliderId == 3)
			m_fThicknessRelativeY = -70.0f;
		m_pTarget = BulletTarget::create(m_iColliderId);
		this->addChild(m_pTarget);
	}

	RectPoly poly{
		Vec2{ -m_ThicknessBoxSize.width / 2.0f, -m_ThicknessBoxSize.height / 2.0f},
		Size{m_ThicknessBoxSize.width, m_ThicknessBoxSize.height }
	};

	// 앵커포인터 0, 0으로 초기화됨; 스프라이트만 0.5인듯
	// 제발 살려줘 코코스야..
	m_pThicknessBox = cocos2d::DrawNode::create();	
	m_pThicknessBox->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);	// 필요업긴함
	m_pThicknessBox->setPositionY(m_fThicknessRelativeY);
	m_pThicknessBox->setOpacity(125);
	m_pThicknessBox->drawPolygon(poly.source(), 4, {}, 1, Color4F::MAGENTA);
	this->addChild(m_pThicknessBox);

	this->scheduleUpdate();
	return true;
}



cocos2d::Rect Collider::getThicknessBox() {
	Vec2 thicknessAbsoluteOrigin = getRealPos();
	Rect thicknessRect{ thicknessAbsoluteOrigin, Size{m_ThicknessBoxSize.width, m_ThicknessBoxSize.height} };
	return thicknessRect;
}

cocos2d::Rect Collider::getHitBox() {
	// 위치: 캔버스 좌하단 절대 좌표 + 캔버스 좌하단 기준 스킨 파츠 좌표
	//      캔버스 좌하단 절대 좌표 = 플레이어 Cocos 위치  + 캐릭터 위치 - (캔버스 사이즈 / 2)
	Vec2 absoluteOrigin = (this->getPosition() - m_pTarget->getCanvasSize() / 2);  // 캔버스 좌하단 절대 좌표
	Vec2 skinPartPos = m_pTarget->getSkinPart()->getPosition(); // 캔버스 좌하단 기준 스킨 파츠 좌표
	DrawNode* pSkinBox = m_pTarget->getSkinBoundingBox();
	Size skinSize = pSkinBox->getContentSize();
	Rect hitBox{ absoluteOrigin + m_pTarget->getPosition() + skinPartPos, skinSize };
	return hitBox;
}

void Collider::setRealPos(float x, float y) {
	x += m_ThicknessBoxSize.width / 2.0f;
	y -= m_fThicknessRelativeY;
	y += m_ThicknessBoxSize.height / 2.0f;
	setPosition(x, y);
}

void Collider::setRealPosCenter(float x, float y) {
	setPosition(x, y -= m_fThicknessRelativeY);
}

cocos2d::Vec2 Collider::getRealPos() {	// 두께박스 좌하단 좌표
	// 현재 컬라이더의 위치
	Vec2 thicknessOrigin = getPosition();
	thicknessOrigin.x -= m_ThicknessBoxSize.width / 2.0f;
	thicknessOrigin.y += (m_fThicknessRelativeY - (m_ThicknessBoxSize.height / 2.0f));
	return thicknessOrigin;
}

cocos2d::Vec2 Collider::getRealPosCenter() {
	Vec2 thicknessOrigin = getPosition();
	thicknessOrigin.y += m_fThicknessRelativeY;
	return thicknessOrigin;
}

cocos2d::Vec2 Collider::getTargetCanvasPosition() {
	return (this->getPosition() - m_pTarget->getCanvasSize() / 2) + m_pTarget->getPosition();  // 캔버스 좌하단 절대 좌표
}

void Collider::playFrameEvent(FrameEvent* frameEvent) {
	if (frameEvent == nullptr)
		return;

}

void Collider::createProjectile(int projectileId) {

	// 가상의 코드...
	// 이런느낌으로 가자
	// ProjectileInfo* pInfo = Loader[projectileId];
	// Size thicknessSize = pInfo->thicknessSize
	// float relativeY = pInfo->relativeY;
	// int soundId = pInfo->soundId;
	// float rotation =

	auto collider = Collider::create(ColliderType::Projectile, projectileId);
	collider->dir = ((Player*)this)->getController()->getSpriteDirection();
	Vec2 offset;
	if (projectileId == 0)
		offset = { 372, 238 };
	else if (projectileId == 1)
		offset = { 386, 260 };
	else if (projectileId == 2) {
		offset = { 346, 196 };
	} else if (projectileId == 3) {
		offset = { 384, 191 };
	}

	
	auto targetCanvasPos = this->getTargetCanvasPosition();	// 플레이어는 캐릭터임
	if (collider->dir == 1) {
		collider->setRotationSkewY(180.0f);
		collider->setPosition(targetCanvasPos.x + 500.0f - offset.x, targetCanvasPos.y + offset.y);
	} else {
		collider->setRotationSkewY(0.0f);
		collider->setPosition(targetCanvasPos.x + offset.x, targetCanvasPos.y + offset.y);
	}
	collider->setAnchorPoint(Vec2::ZERO);
	
	World::getInstance()->addCollider(collider);
}

bool Collider::isAttackable(
	const Rect& srcThick, const Rect& targetThick,
	const Rect& srcHit, const Rect& targetHit) {
	auto targetLB = targetThick.getLeftBottom();
	auto targetRT = targetThick.getRightTop();

	if (srcThick.origin.y > targetLB.y &&
		srcThick.origin.y < targetLB.y)
		return true;

	if (srcThick.origin.y + srcThick.size.height > targetRT.y &&
		srcThick.origin.y + srcThick.size.height < targetRT.y)
		return true;

	if (srcHit.intersectsRect(targetHit))
		return true;


	

	return false;
}

void Collider::update(float delta) {

	if (m_iColliderType != ColliderType::Projectile)
		return;

	Vec2 ori = m_pTarget->getPosition();
	float rot = ((BulletTarget*)m_pTarget)->getPartRotation();

	float fMoveDistanceFPS = BulletSpeedFPS_v * cosf(CC_DEGREES_TO_RADIANS(rot));

	if (dir == SpriteDirection::Right)
		this->setPositionX(this->getPositionX() + fMoveDistanceFPS);
	else
		this->setPositionX(this->getPositionX() - fMoveDistanceFPS);
	
	m_pTarget->setPosition(
		m_pTarget->getPositionX(), // ori.x + BulletSpeedFPS_v * cosf(CC_DEGREES_TO_RADIANS(rot)),
		ori.y - BulletSpeedFPS_v * sinf(CC_DEGREES_TO_RADIANS(rot)));

	m_fMoveDistance += fMoveDistanceFPS;


	// 근데 이렇게 update에서 충돌체크 하는게 맞나..
	// 어디 공통적인 장소에서 해야할 것 같은데
	if (World::getInstance()->isCollide(this)) {
		World::getInstance()->removeProjectile(this);
		return;
	}


	// 사거리
	if (m_fMoveDistance >= 400.0f) {
		World::getInstance()->removeProjectile(this);
		return;
	}

	if (m_pTarget->getPositionY() <= m_fThicknessRelativeY) {
		World::getInstance()->removeProjectile(this);
	}

	
}

// this가 col과 충돌했는지
bool Collider::isCollide(Collider* col) {
	Rect my = this->getThicknessBox();
	Rect target = col->getThicknessBox();

	if (!isInsersectThicknessY(my, target)) {
		return false;
	}

	Rect myHit = getHitBox();
	Rect targetHit = col->getHitBox();
	return myHit.intersectsRect(targetHit);
}

bool Collider::isInsersectThicknessY(Collider* lhs, Collider* rhs) {
	return isInsersectThicknessY(lhs->getThicknessBox(), rhs->getThicknessBox());
}

bool Collider::isInsersectThicknessY(const cocos2d::Rect& lhsThick, const cocos2d::Rect& rhsThick) {
	if (lhsThick.origin.y > rhsThick.origin.y &&
		lhsThick.origin.y < rhsThick.origin.y + rhsThick.size.height)
		return true;

	if (lhsThick.origin.y + lhsThick.size.height > rhsThick.origin.y &&
		lhsThick.origin.y + lhsThick.size.height < rhsThick.origin.y + rhsThick.size.height)
		return true;

	return false;
}

bool Collider::isInsersectThicknessX(Collider* lhs, Collider* rhs) {
	return isInsersectThicknessX(lhs->getThicknessBox(), rhs->getThicknessBox());
}

bool Collider::isInsersectThicknessX(const cocos2d::Rect& lhsThick, const cocos2d::Rect& rhsThick) {
	if (lhsThick.origin.x > rhsThick.origin.x &&
		lhsThick.origin.x < rhsThick.origin.x + rhsThick.origin.x + rhsThick.size.width)
		return true;

	if (lhsThick.origin.x + lhsThick.size.width > rhsThick.origin.x &&
		lhsThick.origin.x + lhsThick.size.width < rhsThick.origin.x + rhsThick.size.width)
		return true;

	return false;
}


bool Collider::isInsersectThicknessY(Collider* rhs) {
	return isInsersectThicknessY(this->getThicknessBox(), rhs->getThicknessBox());
}
bool Collider::isInsersectThicknessY(const cocos2d::Rect& rhsThick) {
	return isInsersectThicknessY(this->getThicknessBox(), rhsThick);
}

bool Collider::isInsersectThicknessX(Collider* rhs) {
	return isInsersectThicknessX(this->getThicknessBox(), rhs->getThicknessBox());
}
bool Collider::isInsersectThicknessX(const cocos2d::Rect& rhsThick) {
	return isInsersectThicknessX(this->getThicknessBox(), rhsThick);
}


// Projectile::create(ColliderInfo* info)

struct ColliderInfo
{
	int Type;
	int Id;
	int FrameWidth;
	int FrameHeight;

};



class Projectile : public Collider
{
public:
	// type, 프로젝틸 타입
	// id, 프로젝틸중에 뭔지
	Projectile(Collider* creator, int type, int id) : Collider(type, id){
		
	}


};