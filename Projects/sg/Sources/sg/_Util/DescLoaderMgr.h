/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 6:39:51 PM
 * =====================
 *
 */


#pragma once

#include <sg/_Util/DescLoaderAbstract.h>

struct MonsterBaseInfo;
struct ItemAvatarInfo;
struct ItemWeaponInfo;
struct ItemArmorInfo;
struct ItemVisualInfo;
struct ItemOptInfo;
struct ItemOptInfo;
struct ChannelBaseInfo;
struct EnchantInfo;
struct ServerProcessInfoPackage;
struct DatabaseInfo;
struct CharCommonInfo;
struct MapInfo;
struct MapAreaInfo;
struct MapPhysicsInfo;


// ------------------ 클라것들
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

class DescLoaderMgr : public jc::SingletonPointer<DescLoaderMgr>
{
public:
	DescLoaderMgr();
	virtual ~DescLoaderMgr();

	void LoadAll();
	bool Load(ConfigFileType_t _configFileType);
	void Unload(ConfigFileType_t _configFileType);
	void Clear();	// unload + detete

	void AddLoader(DescLoaderAbstract* _pLoader);

	SDescBase* GetData(ConfigFileType_t _configFileType, int _code);
	SDescBase* GetDataAny(ConfigFileType_t _configFileType);

	// ------------------ 기본
	MonsterBaseInfo*				GetMobBaseInfo(int _monsterCode);
	ItemAvatarInfo*					GetAvatarInfo(int _avatarCode);
	ItemWeaponInfo*					GetWeaponInfo(int _weaponCode);
	ItemArmorInfo*					GetArmorInfo(int _armorCode);
	ItemVisualInfo*					GetVisualInfo(int _visualCode);
	ItemOptInfo*					GetItemOptInfo(int _itemOptCode);
	ItemOptInfo*					GetItemOptInfo(const jc::String& _itemOptEngName);
	ChannelBaseInfo*				GetChannelBaseInfo(int _channelCode);
	EnchantInfo*					GetEnchantInfo(int _enchantCode);
	ServerProcessInfo*				GetServerProcessInfo();
	DatabaseInfo*					GetDatabaseInfo(int _databaseCode);
	CharCommonInfo*					GetCharCommonInfo(int _charCommonCode);
	MapInfo*						GetMapInfo(int _mapCode);
	MapAreaInfo*					GetMapAreaInfo(int _mapCode);
	MapPhysicsInfo*					GetMapPhysicsInfo(int _physicsCode);

	// ------------------ 클라것들
	MonsterInfo*					GetMonsterInfo(int _mobCode);
	ActionInfo*						GetActionInfo(int _actionCode);
	ProjectileInfo*					GetProjectileInfo(ActorType_t _actorType, int _projectileCode);
	CharInfo*						GetCharInfo(int _charCode);
	AnimationInfo*					GetCharAnimationInfo(int _charAnimationCode);
	jc::Vector<AnimationInfo*>&		GetCharAnimationInfoList(int _charCode);
	TileInfo*						GetTileInfo(int _tileCode);
	MapObjectInfo*					GetMapObjectInfo(int _mapObjectCode);
	AIInfo*							GetAiInfo(int _aiCode);
	AttackDataInfo*					GetAttackDataInfo(ActorType_t _actorType, int _attackDataCode);
	EffectInfo*						GetEffectInfo(int _effectCode);
	UIElementInfo*					GetUIElementInfo(int _uiElementCode);
	FrameEvent*						GetFrameEvent(ActorType_t _actorType, int _frameEventCode);
	ChannelInfo*					GetChannelInfo(int _channelCode);

	char*							GetTextRaw(const char* _textId);
	jc::String&						GetText(const char* _textId);
	jc::String&						GetText(const jc::String& _textId);

private:
	DescLoaderAbstract* m_pConfigFileLoaders[ConfigFileType::Max]{};
};

#define g_cDescMgr JC_DECL_SINGLETON_BODY(DescLoaderMgr)