/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 6:39:51 PM
 * =====================
 *
 */


#pragma once

#include "Core.h"

#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>
#include <SteinsGate/Common/SGMobBaseInfo.h>

class SGDataManagerAbstract
{
public:
	SGDataManagerAbstract();
	virtual ~SGDataManagerAbstract();

	virtual void initializeLoader() = 0;
	void loadAll();

	ConfigDataAbstract* getData(ConfigFileType_t configFileType, int code);

	void load(ConfigFileType_t configFileType);
	
	void unload(ConfigFileType_t configFileType); 
	void finalizeLoader();

	SGMobBaseInfo* getMobBaseInfo(int code);
protected:
	ConfigFileLoaderAbstract* m_pConfigFileLoaders[ConfigFileType::Max];
	bool m_bLoaded[ConfigFileType::Max]; // Lazy Loading을 기본으로 하자.
	bool m_bInitialized;
};



