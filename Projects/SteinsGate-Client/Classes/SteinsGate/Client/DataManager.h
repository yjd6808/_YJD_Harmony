/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:11:17 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/DataManagerAbstract.h>

#include <SteinsGate/Common/ItemInfo.h>
#include <SteinsGate/Client/MobInfo.h>
#include <SteinsGate/Client/ActionInfo.h>
#include <SteinsGate/Client/CharInfo.h>
#include <SteinsGate/Client/ProjectileInfo.h>
#include <SteinsGate/Client/ClientInfo.h>
#include <SteinsGate/Client/TileInfo.h>
#include <SteinsGate/Client/MapObjectInfo.h>
#include <SteinsGate/Client/MapInfo.h>
#include <SteinsGate/Client/AIInfo.h>
#include <SteinsGate/Client/AttackDataInfo.h>
#include <SteinsGate/Client/EffectInfo.h>
#include <SteinsGate/Client/UIInfo.h>
#include <SteinsGate/Client/FrameEvent.h>

class DataManager
	: public DataManagerAbstract
	, public JCore::SingletonPointer<DataManager>
{
	friend class TSingleton;
	DataManager();
	~DataManager() override = default;
public:
	void initializeLoader() override;

	MobInfo* getMonsterInfo(int mobCode);
	ActionInfo* getActionInfo(int actionCode);
	ProjectileInfo* getProjectileInfo(ActorType_t actorType, int projectileCode);
	CharInfo* getCharInfo(int charCode);
	AnimationInfo* getCharAnimationInfo(int charAnimationCode);
	SGVector<AnimationInfo*>& getCharAnimationInfoList(int charCode);
	ClientInfo* getClientInfo(int code);
	TileInfo* getTileInfo(int tileCode);
	MapObjectInfo* getMapObjectInfo(int mapObjectCode);
	MapInfo* getMapInfo(int mapCode);
	AIInfo* getAIInfo(int aiCode);
	AttackDataInfo* getAttackDataInfo(ActorType_t actorType, int attackDataCode);
	EffectInfo* getEffectInfo(int effectCode);
	UIElementInfo* getUIElementInfo(int uiElementCode);
	FrameEvent* getFrameEvent(ActorType_t actorType, int frameEventCode);

};

