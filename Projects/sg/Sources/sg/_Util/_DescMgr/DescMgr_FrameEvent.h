/*
 * 작성자: 윤정도
 * 생성일: 7/23/2023 4:27:54 AM
 * =====================
 *
 */

#pragma once

#include "sg/_Struct/SteinsGate_FrameEvent.h"
#include "sg/_Util/DescLoaderAbstract.h"

struct FrameEventLoader : DescLoaderAbstract
{
public:
	FrameEventLoader(ActorType_t _actorType);
	~FrameEventLoader() override = default;

	bool Load() override;

	ConfigFileType_t GetConfigFileType() override
	{
		switch (actorType_)
		{
		case ActorType::Character:
			return ConfigFileType::Char_Animation_Frame_Event;
		case ActorType::Monster:
			return ConfigFileType::Monster_Animation_Frame_Event;
		default:
			jc_assert(false);
		}

		return ConfigFileType::Max;
	}

	static void ReadFrameEventSpawn(Json::Value& _frameEventRoot, OUT FrameEventSpawn* _pFrameEvent);
	static void ReadFrameEventAttackBoxInstant(Json::Value& _frameEventRoot, OUT FrameEventAttackBoxInstant* _pFrameEvent);

private:
	ActorType_t actorType_; // 어떤 액터의 데이터인지 구분하는 용도
};
