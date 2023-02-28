/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 6:39:51 PM
 * =====================
 *
 */


#pragma once

#include "Core.h"

#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>
#include <SteinsGate/Common/MobBaseInfo.h>
#include <SteinsGate/Common/ItemInfo.h>

#include <SteinsGate/Common/ChannelInfo.h>
#include <SteinsGate/Common/ItemInfo.h>
#include <SteinsGate/Common/ItemOptInfo.h>
#include <SteinsGate/Common/EnchantInfo.h>
#include <SteinsGate/Common/ServerInfo.h>

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

	MobBaseInfo* getMobBaseInfo(int monsterCode);
	ItemAvatarInfo* getAvatarInfo(int avatarCode);		// 아바타
	ItemWeaponInfo* getWeaponInfo(int weaponCode);		// 무기
	ItemArmorInfo* getArmorInfo(int armorCode);			// 장신구 혹은 방어구
	ItemVisualInfo* getVisualInfo(int visualCode);		// 아바타 혹은 무기;
	CommonInfo* getCommonInfo(int commonConfigCode);	// 커몬 컨피크

	ItemOptInfo* getItemOptInfo(int itemOptCode);
	ItemOptInfo* getItemOptInfo(const SGString& itemOptEngName);
	ChannelInfo* getChannelInfo(int channelCode);
	EnchantInfo* getEnchantInfo(int enchantCode);
	ServerInfo* getServerInfo(int serverCode);
protected:
	ConfigFileLoaderAbstract* m_pConfigFileLoaders[ConfigFileType::Max];
	bool m_bLoaded[ConfigFileType::Max]; // Lazy Loading을 기본으로 하자.
	bool m_bInitialized;
};


