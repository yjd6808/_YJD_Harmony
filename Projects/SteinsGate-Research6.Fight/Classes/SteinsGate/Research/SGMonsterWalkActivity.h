/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */


#pragma once


#include "Tutturu.h"
#include <SteinsGate/Research/SGMonsterActivity.h>
#include <SteinsGate/Research/SGMapInfo.h>

class SGActor;
class SGMapLayer;
class SGMonster;
class SGMonsterWalkActivity : public SGMonsterActivity
{
public:
	enum Mode
	{
		eWander,
		eTrack
	};

	SGMonsterWalkActivity(SGMonster* monster);

	void onActivityBegin() override;
	void onUpdate(float dt) override;

	void updateWander(float dt);
	void updateTrack(float dt);
	void updateMove(float dt, SGMapLayer* pMapLayer);
	void updateLeftMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& thicknessRect);
	void updateRightMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& thicknessRect);
	void updateUpMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& thicknessRect);
	void updateDownMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& thicknessRect);

	void setDestination(const SGVec2& destination);
	void setTarget(SGActor* target);
	void setMode(Mode mode) { m_eMode = mode; }
protected:
	SGVec2 m_Destination;
	SGActor* m_pTarget;
	Mode m_eMode;
};


