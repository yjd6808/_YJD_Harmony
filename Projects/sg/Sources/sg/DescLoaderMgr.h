/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 6:39:51 PM
 * =====================
 *
 */


#pragma once

#include <sg/DescLoaderAbstract.h>


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

class DescLoaderMgr
{
public:
	DescLoaderMgr();
	virtual ~DescLoaderMgr();

	void LoadAll();
	bool Load(ConfigFileType_t _configFileType);
	void Unload(ConfigFileType_t _configFileType);
	void FinalizeLoader();

	SDescBase*				GetData(ConfigFileType_t _configFileType, int _code);
	MonsterBaseInfo*				GetMobBaseInfo(int _monsterCode);
	ItemAvatarInfo*					GetAvatarInfo(int _avatarCode);
	ItemWeaponInfo*					GetWeaponInfo(int _weaponCode);
	ItemArmorInfo*					GetArmorInfo(int _armorCode);
	ItemVisualInfo*					GetVisualInfo(int _visualCode);
	ItemOptInfo*					GetItemOptInfo(int _itemOptCode);
	ItemOptInfo*					GetItemOptInfo(const jc::String& _itemOptEngName);
	ChannelBaseInfo*				GetChannelBaseInfo(int _channelCode);
	EnchantInfo*					GetEnchantInfo(int _enchantCode);
	ServerProcessInfoPackage*		GetServerProcessInfoPackage(int _serverCode);
	DatabaseInfo*					GetDatabaseInfo(int _databaseCode);
	CharCommonInfo*					GetCharCommonInfo(int _charCommonCode);
	MapInfo*						GetMapInfo(int _mapCode);
	MapAreaInfo*					GetMapAreaInfo(int _mapCode);
	MapPhysicsInfo*					GetMapPhysicsInfo(int _physicsCode);

protected:
	DescLoaderAbstract*		m_pConfigFileLoaders[ConfigFileType::Max];
};
