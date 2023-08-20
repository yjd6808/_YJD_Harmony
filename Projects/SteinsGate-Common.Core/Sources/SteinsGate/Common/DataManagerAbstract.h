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

	ConfigDataAbstract* getData(ConfigFileType_t configFileType, int code);

	
	void load(ConfigFileType_t configFileType);
	void unload(ConfigFileType_t configFileType); 
	void finalizeLoader();

	MonsterBaseInfo* getMobBaseInfo(int monsterCode);
	ItemAvatarInfo* getAvatarInfo(int avatarCode);		// 아바타
	ItemWeaponInfo* getWeaponInfo(int weaponCode);		// 무기
	ItemArmorInfo* getArmorInfo(int armorCode);			// 장신구 혹은 방어구
	ItemVisualInfo* getVisualInfo(int visualCode);		// 아바타 혹은 무기;
	CommonInfo* getCommonInfo(int commonConfigCode);	// 커몬 컨피크

	ItemOptInfo* getItemOptInfo(int itemOptCode);
	ItemOptInfo* getItemOptInfo(const SGString& itemOptEngName);
	ChannelBaseInfo* getChannelBaseInfo(int channelCode);
	EnchantInfo* getEnchantInfo(int enchantCode);
	ServerProcessInfoPackage* getServerProcessInfoPackage(int serverCode);
	DatabaseInfo* getDatabaseInfo(int databaseCode);
	CharCommonInfo* getCharCommonInfo(int charCommonCode);
	MapInfo* getMapInfo(int mapCode);
	MapAreaInfo* getMapAreaInfo(int mapCode);
	MapPhysicsInfo* getMapPhysicsInfo(int physicsCode);
protected:
	ConfigFileLoaderAbstract* m_pConfigFileLoaders[ConfigFileType::Max];
	bool m_bLoaded[ConfigFileType::Max]; // Lazy Loading을 기본으로 하자.
	bool m_bInitialized;
};



