/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/AIActivity.h>

class JCORE_NOVTABLE AttackActivity : public AIActivity
{
public:
	AttackActivity(Actor* actor);
protected:
 	JCORE_NULLABLE HitRecorder* m_pHitRecorder; // 히트 레코더가 없는 객체도 있음
};




