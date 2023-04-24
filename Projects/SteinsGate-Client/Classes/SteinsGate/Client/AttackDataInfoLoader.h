/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:09 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/AttackDataInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct AttackDataInfoLoader : ConfigFileLoaderAbstract
{
public:
	AttackDataInfoLoader(DataManagerAbstract* manager);
	~AttackDataInfoLoader() override = default;

	bool load() override;
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::AttackData; }

	static void readAttackDataInfo(Json::Value& attackDataRoot, Out_ AttackDataInfo* attackDataInfo);
};


