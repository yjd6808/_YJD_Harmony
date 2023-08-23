/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:11:17 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/DataManagerAbstract.h>

#include <SteinsGate/Common/ItemInfo.h>
#include <SteinsGate/Client/MonsterInfo.h>
#include <SteinsGate/Client/ActionInfo.h>
#include <SteinsGate/Client/CharInfo.h>
#include <SteinsGate/Client/ProjectileInfo.h>
#include <SteinsGate/Client/ClientInfo.h>
#include <SteinsGate/Client/TileInfo.h>
#include <SteinsGate/Client/MapObjectInfo.h>
#include <SteinsGate/Client/AIInfo.h>
#include <SteinsGate/Client/AttackDataInfo.h>
#include <SteinsGate/Client/EffectInfo.h>
#include <SteinsGate/Client/UIInfo.h>
#include <SteinsGate/Client/FrameEvent.h>
#include <SteinsGate/Client/ChannelInfo.h>

class DataManager
	: public DataManagerAbstract
	, public JCore::SingletonPointer<DataManager>
{
	friend class TSingleton;
	DataManager();
	~DataManager() override = default;
public:
	void initializeLoader() override;

	MonsterInfo* getMonsterInfo(int mobCode);
	ActionInfo* getActionInfo(int actionCode);
	ProjectileInfo* getProjectileInfo(ActorType_t actorType, int projectileCode);
	CharInfo* getCharInfo(int charCode);
	AnimationInfo* getCharAnimationInfo(int charAnimationCode);
	SGVector<AnimationInfo*>& getCharAnimationInfoList(int charCode);
	ClientInfo* getClientInfo(int code);
	TileInfo* getTileInfo(int tileCode);
	MapObjectInfo* getMapObjectInfo(int mapObjectCode);
	AIInfo* getAIInfo(int aiCode);
	AttackDataInfo* getAttackDataInfo(ActorType_t actorType, int attackDataCode);
	EffectInfo* getEffectInfo(int effectCode);
	UIElementInfo* getUIElementInfo(int uiElementCode);
	FrameEvent* getFrameEvent(ActorType_t actorType, int frameEventCode);
	ChannelInfo* getChannelInfo(int channelCode);

	char* getTextRaw(const char* szId);
	SGString& getText(const char* szId);
	SGString& getText(const SGString& szId);
};

#define SG_TEXT_RAW(id)					Core::DataManager->getTextRaw(id)
#define SG_TEXT(id)						Core::DataManager->getText(id)

#define SG_TEXT_RAW_FMT(id, ...)		JCore::StringUtil::Format(Core::DataManager->getTextRaw(id), __VA_ARGS__)
#define SG_TEXT_RAW_FMT_STD(id, ...)	StringUtils::format(Core::DataManager->getTextRaw(id), __VA_ARGS__)
