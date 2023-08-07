/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include "Tutturu.h"
#include <SteinsGate/Common/MapInfo.h>
#include <SteinsGate/Client/MonsterActivity.h>


class Actor;
class MapLayer;
class Monster;
class MonsterWalkActivity : public MonsterActivity
{
public:
	enum Mode
	{
		eWander,
		eTrack
	};

	MonsterWalkActivity(Monster* monster);

	void onActivityBegin() override;
	void onUpdate(float dt) override;

	void updateWander(float dt);
	void updateTrack(float dt);
	void updateMove(float dt, MapLayer* pMapLayer);
	void updateLeftMove(MapLayer* mapLayer, MapAreaInfo* mapInfo, const SGRect& thicknessRect);
	void updateRightMove(MapLayer* mapLayer, MapAreaInfo* mapInfo, const SGRect& thicknessRect);
	void updateUpMove(MapLayer* mapLayer, MapAreaInfo* mapInfo, const SGRect& thicknessRect);
	void updateDownMove(MapLayer* mapLayer, MapAreaInfo* mapInfo, const SGRect& thicknessRect);

	void setDestination(const SGVec2& destination);
	void setTarget(Actor* target);
	void setMode(Mode mode) { m_eMode = mode; }
protected:
	SGVec2 m_Destination;
	Actor* m_pTarget;
	Mode m_eMode;
};


