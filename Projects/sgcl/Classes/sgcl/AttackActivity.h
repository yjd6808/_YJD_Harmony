/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */

#pragma once


#include <sgcl/AIActivity.h>

class JCORE_NOVTABLE AttackActivity : public AIActivity
{
public:
	AttackActivity(Actor* _pActor);

protected:
	NULLABLE HitRecorder* pHitRecorder_; // 히트 레코더가 없는 객체도 있음
};
