/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 1:30:10 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/SGUIInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct SGUIInfoLoader : ConfigFileLoaderAbstract
{
public:
	SGUIInfoLoader(DataManagerAbstract* manager);
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::UI; }
	bool load() override;

	static void readElementCommon(Json::Value& elementRoot, Out_ SGUIElementInfo* elementInfo);
	static void readElementGroup(Json::Value& uiGroupRoot, Out_ SGUIGroupInfo* groupInfo);
	static SGUIElementInfo* readElementButton(Json::Value& buttonRoot);
	static SGUIElementInfo* readElementLabel(Json::Value& labelRoot);
	static SGUIElementInfo* readElementSprite(Json::Value& spriteRoot);
	
};



