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
	AttackDataInfoLoader(DataManagerAbstract* _pManager, ActorType_t _actorType);
	~AttackDataInfoLoader() override = default;

	bool Load() override;

	ConfigFileType_t GetConfigFileType() override
	{
		switch (actorType_)
		{
		case ActorType::Character: return ConfigFileType::Char_Attack_Data;
		case ActorType::Monster: return ConfigFileType::Monster_Attack_Data;
		default: DebugAssert(false);
		}
		return ConfigFileType::Max;
	}

	static void ReadAttackDataInfo(Json::Value& _attackDataRoot, JCORE_OUT AttackDataInfo* _pAttackDataInfo);

private:
	ActorType_t actorType_; // 어떤 액터의 데이터인지 구분하는 용도
};
