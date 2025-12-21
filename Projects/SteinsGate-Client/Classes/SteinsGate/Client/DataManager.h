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

	MonsterInfo* GetMonsterInfo(int _mobCode);
	ActionInfo* GetActionInfo(int _actionCode);
	ProjectileInfo* GetProjectileInfo(ActorType_t _actorType, int _projectileCode);
	CharInfo* GetCharInfo(int _charCode);
	AnimationInfo* GetCharAnimationInfo(int _charAnimationCode);
	SGVector<AnimationInfo*>& GetCharAnimationInfoList(int _charCode);
	ClientInfo* GetClientInfo(int _code);
	TileInfo* GetTileInfo(int _tileCode);
	MapObjectInfo* GetMapObjectInfo(int _mapObjectCode);
	AIInfo* GetAiInfo(int _aiCode);
	AttackDataInfo* GetAttackDataInfo(ActorType_t _actorType, int _attackDataCode);
	EffectInfo* GetEffectInfo(int _effectCode);
	UIElementInfo* GetUiElementInfo(int _uiElementCode);
	FrameEvent* GetFrameEvent(ActorType_t _actorType, int _frameEventCode);
	ChannelInfo* GetChannelInfo(int _channelCode);

	char* GetTextRaw(const char* _textId);
	SGString& GetText(const char* _textId);
	SGString& GetText(const SGString& _textId);
};

#define SG_TEXT_RAW(id)					Core::DataManager->GetTextRaw(id)
#define SG_TEXT(id)						Core::DataManager->GetText(id)

#define SG_TEXT_RAW_FMT(id, ...)		JCore::StringUtil::Format(Core::DataManager->GetTextRaw(id), __VA_ARGS__)
#define SG_TEXT_RAW_FMT_STD(id, ...)	StringUtils::format(Core::DataManager->GetTextRaw(id), __VA_ARGS__)
