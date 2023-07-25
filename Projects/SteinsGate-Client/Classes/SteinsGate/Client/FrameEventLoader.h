/*
 * 작성자: 윤정도
 * 생성일: 7/23/2023 4:27:54 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Client/FrameEvent.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct FrameEventLoader : ConfigFileLoaderAbstract
{
public:
	FrameEventLoader(DataManagerAbstract* manager, ActorType_t actorType);
	~FrameEventLoader() override = default;

	bool load() override;
	ConfigFileType_t getConfigFileType() override {
		switch (m_eActorType) {
		case ActorType::Character:	return ConfigFileType::Char_Animation_Frame_Event;
		case ActorType::Monster:	return ConfigFileType::Monster_Animation_Frame_Event;
		default: DebugAssert(false);
		}
		return ConfigFileType::Max;
	}

	static void readFrameEventSpawn(Json::Value& frameEventRoot, JCORE_OUT FrameEventSpawn* frameEvent);
	static void readFrameEventAttackBoxInstant(Json::Value& frameEventRoot, JCORE_OUT FrameEventAttackBoxInstant* frameEvent);
private:
	ActorType_t m_eActorType;		// 어떤 액터의 데이터인지 구분하는 용도
};


