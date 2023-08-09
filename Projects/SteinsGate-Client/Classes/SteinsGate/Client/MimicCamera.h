/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 6:26:21 AM
 * =====================
 *
 * 카메라를 다를줄몰라서 만든거..
 */


#pragma once


#include <SteinsGate/Client/Tutturu.h>


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




