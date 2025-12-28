/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 6:39:51 PM
 * =====================
 *
 */


#pragma once

#include "Core.h"

#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>
#include <SteinsGate/Common/MonsterBaseInfo.h>
#include <SteinsGate/Common/ItemInfo.h>
#include <SteinsGate/Common/MapInfo.h>
#include <SteinsGate/Common/MapPhysicsInfo.h>
#include <SteinsGate/Common/ChannelBaseInfo.h>
#include <SteinsGate/Common/ItemInfo.h>
#include <SteinsGate/Common/ItemOptInfo.h>
#include <SteinsGate/Common/EnchantInfo.h>
#include <SteinsGate/Common/ServerProcessInfo.h>
#include <SteinsGate/Common/DatabaseInfo.h>
#include <SteinsGate/Common/CharCommonInfo.h>

class DataManagerAbstract
{
public:
	DataManagerAbstract();
	virtual ~DataManagerAbstract();

	virtual void InitializeLoader() = 0;

	void LoadCommon();
	void LoadAll();

	ConfigDataAbstract* GetData(ConfigFileType_t _configFileType, int _code);

	void Load(ConfigFileType_t _configFileType);
	void Unload(ConfigFileType_t _configFileType);
	void FinalizeLoader();

	MonsterBaseInfo* GetMobBaseInfo(int _monsterCode);
	ItemAvatarInfo* GetAvatarInfo(int _avatarCode);
	ItemWeaponInfo* GetWeaponInfo(int _weaponCode);
	ItemArmorInfo* GetArmorInfo(int _armorCode);
	ItemVisualInfo* GetVisualInfo(int _visualCode);
	CommonInfo* GetCommonInfo(int _commonConfigCode);

	ItemOptInfo* GetItemOptInfo(int _itemOptCode);
	ItemOptInfo* GetItemOptInfo(const SGString& _itemOptEngName);
	ChannelBaseInfo* GetChannelBaseInfo(int _channelCode);
	EnchantInfo* GetEnchantInfo(int _enchantCode);
	ServerProcessInfoPackage* GetServerProcessInfoPackage(int _serverCode);
	DatabaseInfo* GetDatabaseInfo(int _databaseCode);
	CharCommonInfo* GetCharCommonInfo(int _charCommonCode);
	MapInfo* GetMapInfo(int _mapCode);
	MapAreaInfo* GetMapAreaInfo(int _mapCode);
	MapPhysicsInfo* GetMapPhysicsInfo(int _physicsCode);

protected:
	ConfigFileLoaderAbstract* m_pConfigFileLoaders[ConfigFileType::Max];
	bool m_bLoaded[ConfigFileType::Max];
	bool m_bInitialized;
};
