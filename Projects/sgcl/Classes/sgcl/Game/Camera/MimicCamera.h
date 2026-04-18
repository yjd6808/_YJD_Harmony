/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 6:26:21 AM
 * =====================
 *
 * 카메라를 다를줄몰라서 만든거..
 */

#pragma once


class MapLayer;
class Actor;

class MimicCamera : public cc::Node
{
public:
	MimicCamera();

	static MimicCamera* Create();

	void SetFollowTarget(Actor* _pTarget);
	void UpdateView(float _dt);
	void update(float _dt) override;

private:
	Actor* pTarget_;
	MapLayer* pMapLayer_;
};
