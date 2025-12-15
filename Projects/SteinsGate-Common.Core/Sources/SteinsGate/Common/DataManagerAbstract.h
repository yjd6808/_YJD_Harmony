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

	virtual void initializeLoader() = 0;

	void loadCommon();
	void loadAll();

	ConfigDataAbstract* getData(ConfigFileType_t _configFileType, int _code);

	void load(ConfigFileType_t _configFileType);
	void unload(ConfigFileType_t _configFileType);
	void finalizeLoader();

	MonsterBaseInfo* getMobBaseInfo(int _monsterCode);
	ItemAvatarInfo* getAvatarInfo(int _avatarCode);
	ItemWeaponInfo* getWeaponInfo(int _weaponCode);
	ItemArmorInfo* getArmorInfo(int _armorCode);
	ItemVisualInfo* getVisualInfo(int _visualCode);
	CommonInfo* getCommonInfo(int _commonConfigCode);

	ItemOptInfo* getItemOptInfo(int _itemOptCode);
	ItemOptInfo* getItemOptInfo(const SGString& _itemOptEngName);
	ChannelBaseInfo* getChannelBaseInfo(int _channelCode);
	EnchantInfo* getEnchantInfo(int _enchantCode);
	ServerProcessInfoPackage* getServerProcessInfoPackage(int _serverCode);
	DatabaseInfo* getDatabaseInfo(int _databaseCode);
	CharCommonInfo* getCharCommonInfo(int _charCommonCode);
	MapInfo* getMapInfo(int _mapCode);
	MapAreaInfo* getMapAreaInfo(int _mapCode);
	MapPhysicsInfo* getMapPhysicsInfo(int _physicsCode);

protected:
	ConfigFileLoaderAbstract* m_pConfigFileLoaders[ConfigFileType::Max];
	bool m_bLoaded[ConfigFileType::Max];
	bool m_bInitialized;
};
