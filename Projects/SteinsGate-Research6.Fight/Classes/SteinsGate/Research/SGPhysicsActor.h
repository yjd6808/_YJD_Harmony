/*
 * 작성자: 윤정도
 * 생성일: 1/26/2023 6:58:56 PM
 * =====================
 * 짝퉁 물리가 적용될 액터
 *  - 캐릭터
 *	- 몬스터
 */


#pragma once

#include <SteinsGate/Research/SGActor.h>

class SGPhysicsActor : public SGActor
{
public:
	SGPhysicsActor(ActorType_t type, int code);
	~SGPhysicsActor() override = default;

	bool initVariables() override;

	virtual void hit(const SGHitInfo& hitInfo);
	virtual void hit(SGActor* attacker, const SpriteDirection_t hitDirection, const SGRect& hitRect, SGAttackDataInfo* attackDataInfo);
	virtual void dead();


	bool isPhysicsActor() override { return true; }
	bool isPaused();
	bool isBounced();
	bool isDead();

	void setWeight(float weight) { m_fWeight = weight; }
	void disableElasticity();
	void enableElasticity();

	
	void update(float dt) override;
	void updatePauseTime(float dt);
	void updatePhysics(float dt);
	void updateGravity(float dt);
	void updateFriction(float dt);

	
	// 디버깅용 코드 =====================
	void updateDebug(float dt);
	void updateDebugSub1(float dt);
	void updateDebugSub2(float dt);
	// =================================

	float addForceX(float force);
	float addForceY(float force);

	float removeForceX();
	float removeForceY();

	float getUpTime() { return m_fUpTime; }
	float getDownTime() { return m_fDownTime; }
	float getForceX() { return m_fVelocityX; }
	float getForceY() { return m_fVelocityY; }
	float getWeight() { return m_fWeight; }

	bool hasForceX();
	bool hasForceY();
	bool hasForce();

	void pausePhysics();
	void pausePhysics(float time);
	void resume();

	void stiffenBody(float time);		// 경직시키다 영단어
	

	Direction_t getForceXDirection();
	Direction_t getForceYDirection();

	
protected:
	bool m_bUseElasticity;
	bool m_bBounced;
	bool m_bDead;

	float m_fWeight;
	float m_fUpTime;
	float m_fDownTime;
	float m_fVelocityX;
	float m_fVelocityY;
	float m_fElapsedPausedTime;
	float m_fPuaseTime;

	

	// 디버깅 용
	float m_fAtkBoxInstantElapsedTime;
	SGDrawNode* m_pAtkThicknessBox;
	SGDrawNode* m_pAtkHitBox;
};


