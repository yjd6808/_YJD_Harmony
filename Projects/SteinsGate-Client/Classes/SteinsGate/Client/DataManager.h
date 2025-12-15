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

	MonsterInfo* getMonsterInfo(int _mobCode);
	ActionInfo* getActionInfo(int _actionCode);
	ProjectileInfo* getProjectileInfo(ActorType_t _actorType, int _projectileCode);
	CharInfo* getCharInfo(int _charCode);
	AnimationInfo* getCharAnimationInfo(int _charAnimationCode);
	SGVector<AnimationInfo*>& getCharAnimationInfoList(int _charCode);
	ClientInfo* getClientInfo(int _code);
	TileInfo* getTileInfo(int _tileCode);
	MapObjectInfo* getMapObjectInfo(int _mapObjectCode);
	AIInfo* getAIInfo(int _aiCode);
	AttackDataInfo* getAttackDataInfo(ActorType_t _actorType, int _attackDataCode);
	EffectInfo* getEffectInfo(int _effectCode);
	UIElementInfo* getUIElementInfo(int _uiElementCode);
	FrameEvent* getFrameEvent(ActorType_t _actorType, int _frameEventCode);
	ChannelInfo* getChannelInfo(int _channelCode);

	char* getTextRaw(const char* _textId);
	SGString& getText(const char* _textId);
	SGString& getText(const SGString& _textId);
};

#define SG_TEXT_RAW(id)					Core::DataManager->getTextRaw(id)
#define SG_TEXT(id)						Core::DataManager->getText(id)

#define SG_TEXT_RAW_FMT(id, ...)		JCore::StringUtil::Format(Core::DataManager->getTextRaw(id), __VA_ARGS__)
#define SG_TEXT_RAW_FMT_STD(id, ...)	StringUtils::format(Core::DataManager->getTextRaw(id), __VA_ARGS__)
