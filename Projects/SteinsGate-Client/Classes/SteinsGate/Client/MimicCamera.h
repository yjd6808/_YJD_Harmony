/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 6:26:21 AM
 * =====================
 *
 */


#pragma once


#include "Tutturu.h"


class MapLayer;
class MimicCamera : public SGNode
{
public:
	MimicCamera();

	static MimicCamera* create();

	void setFollowTarget(Actor* actor);
	void updateView(float dt);
	void update(float dt);
private:
	Actor* m_pTarget;
	MapLayer* m_pMapLayer;
};




