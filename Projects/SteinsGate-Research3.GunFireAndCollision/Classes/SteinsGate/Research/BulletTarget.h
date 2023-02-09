/*
 * 작성자: 윤정도
 * 생성일: 1/16/2023 7:31:20 AM
 * =====================
 *
 */


#pragma once
 /*
  * 작성자: 윤정도
  * 생성일: 1/15/2023 10:39:16 PM
  * =====================
  * 테스트용이니까 대충 암거나 만듬
  * 다음 리서치에서 구조 개편해야할 듯
  * 추후 추상클래스로 다룰예정
  */




#pragma once

#include <SteinsGate/Research/Tutturu.h>

#include "ColliderTarget.h"

class Collider;
class BulletTarget : public ColliderTarget {	// Node
public:
	static BulletTarget* create(int projectileId);


	float getPartRotation() { return m_pSkinPart->getRotation(); }
	float setPartRotation(float r) { m_pSkinPart->setRotation(r); }
	bool init(int projectileId);
};


