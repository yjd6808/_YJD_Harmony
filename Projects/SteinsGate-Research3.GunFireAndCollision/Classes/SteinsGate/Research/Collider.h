/*
 * 작성자: 윤정도
 * 생성일: 1/15/2023 9:38:37 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/ColliderTarget.h>

class FrameEvent;
class Collider : public cocos2d::Node {

public:
	Collider(const int type, const int id);

	static Collider* create(const int type, const int id);
	bool init() override;

	virtual cocos2d::Rect getThicknessBox();
	virtual cocos2d::Rect getHitBox();					
	virtual void setRealPos(float x, float y);			// 좌하단의 좌표를 옮김
	virtual void setRealPosCenter(float x, float y);	// 중심의 좌표를 옮김
	virtual cocos2d::Vec2 getRealPos();
	virtual cocos2d::Vec2 getRealPosCenter();			// 중심의 좌표
	virtual cocos2d::Vec2 getTargetCanvasPosition();	// 타겟의 캔버스 절대좌표

	// 두께박스가 실제 위치 기준이다.
	void setRealPos(const cocos2d::Vec2& v) { setRealPos(v.x, v.y); }

	float getRealPosX() { return getRealPos().x; }
	float getRealPosY() { return getRealPos().y; }

	void playFrameEvent(FrameEvent* event);
	void createProjectile(int projectileId);
	static bool isAttackable(
		const cocos2d::Rect& srcThick, const cocos2d::Rect& targetThick,
		const cocos2d::Rect& srcHit, const cocos2d::Rect& targetHit);
	void update(float delta) override;

	bool isCollide(Collider* col);

	bool isInsersectThicknessY(Collider* rhs);
	bool isInsersectThicknessY(const cocos2d::Rect& rhsThick);
	bool isInsersectThicknessX(Collider* rhs);
	bool isInsersectThicknessX(const cocos2d::Rect& rhsThick);

	static bool isInsersectThicknessY(Collider* lhs, Collider* rhs);
	static bool isInsersectThicknessY(const cocos2d::Rect& lhsThick, const cocos2d::Rect& rhsThick);
	static bool isInsersectThicknessX(Collider* lhs, Collider* rhs);
	static bool isInsersectThicknessX(const cocos2d::Rect& lhsThick, const cocos2d::Rect& rhsThick);
protected:
	int m_iColliderType;	// 캐릭터, 몬스터, 장애물 구분
	int m_iColliderId;		// 캐릭터, 몬스터, 쟁아물 상세

	ColliderTarget* m_pTarget;				// 실제 히트박스
	cocos2d::DrawNode* m_pThicknessBox;
	cocos2d::Size m_ThicknessBoxSize;
	float m_fThicknessRelativeY;
	float m_fMoveDistance{};
	float m_fLimitMoveDistance;
	int dir = 0;
};





