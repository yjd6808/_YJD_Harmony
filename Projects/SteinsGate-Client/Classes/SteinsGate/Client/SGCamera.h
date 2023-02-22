/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 6:26:21 AM
 * =====================
 *
 */


#pragma once


#include "Tutturu.h"


class SGMapLayer;
class SGCamera : public SGNode
{
public:
	SGCamera();

	static SGCamera* create();

	void setFollowTarget(SGActor* actor);
	void updateView(float dt);
	void update(float dt);
private:
	SGActor* m_pTarget;
	SGMapLayer* m_pMapLayer;
};




