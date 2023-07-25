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
	AttackDataInfoLoader(DataManagerAbstract* manager, ActorType_t actorType);
	~AttackDataInfoLoader() override = default;

	bool load() override;
	ConfigFileType_t getConfigFileType() override {

		switch (m_eActorType) {
		case ActorType::Character:	return ConfigFileType::Char_Attack_Data;
		case ActorType::Monster:	return ConfigFileType::Monster_Attack_Data;
		default: DebugAssert(false);
		}
		return ConfigFileType::Max;
	}

	static void readAttackDataInfo(Json::Value& attackDataRoot, JCORE_OUT AttackDataInfo* attackDataInfo);
private:
	ActorType_t m_eActorType;		// 어떤 액터의 데이터인지 구분하는 용도
};


