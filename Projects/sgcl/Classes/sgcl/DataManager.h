/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:11:17 AM
 * =====================
 *
 */


#pragma once

#include <sg/DescLoaderMgr.h>

struct MonsterInfo;
struct ActionInfo;
struct ProjectileInfo;
struct CharInfo;
struct AnimationInfo;
struct TileInfo;
struct MapObjectInfo;
struct AIInfo;
struct AttackDataInfo;
struct EffectInfo;
struct UIElementInfo;
struct FrameEvent;
struct ChannelInfo;

class DataManager
	: public DescLoaderMgr
	, public jc::SingletonPointer<DataManager>
{
	friend class TSingleton;
	DataManager();
	~DataManager() override = default;

public:
	MonsterInfo* GetMonsterInfo(int _mobCode);
	ActionInfo* GetActionInfo(int _actionCode);
	ProjectileInfo* GetProjectileInfo(ActorType_t _actorType, int _projectileCode);
	CharInfo* GetCharInfo(int _charCode);
	AnimationInfo* GetCharAnimationInfo(int _charAnimationCode);
	jc::Vector<AnimationInfo*>& GetCharAnimationInfoList(int _charCode);
	TileInfo* GetTileInfo(int _tileCode);
	MapObjectInfo* GetMapObjectInfo(int _mapObjectCode);
	AIInfo* GetAiInfo(int _aiCode);
	AttackDataInfo* GetAttackDataInfo(ActorType_t _actorType, int _attackDataCode);
	EffectInfo* GetEffectInfo(int _effectCode);
	UIElementInfo* GetUIElementInfo(int _uiElementCode);
	FrameEvent* GetFrameEvent(ActorType_t _actorType, int _frameEventCode);
	ChannelInfo* GetChannelInfo(int _channelCode);

	char* GetTextRaw(const char* _textId);
	jc::String& GetText(const char* _textId);
	jc::String& GetText(const jc::String& _textId);
};

#define SG_TEXT_RAW(id)					sg::DataManager->GetTextRaw(id)
#define SG_TEXT(id)						sg::DataManager->GetText(id)

#define SG_TEXT_RAW_FMT(id, ...)		jc::StringUtil::Format(sg::DataManager->GetTextRaw(id), __VA_ARGS__)
#define SG_TEXT_RAW_FMT_STD(id, ...)	StringUtils::format(sg::DataManager->GetTextRaw(id), __VA_ARGS__)
