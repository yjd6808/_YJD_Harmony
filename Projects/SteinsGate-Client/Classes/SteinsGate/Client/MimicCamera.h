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

	void setFollowTarget(Actor* _pTarget);
	void updateView(float _dt);
	void update(float _dt) override;

private:
	Actor* target_;
	MapLayer* mapLayer_;
};
