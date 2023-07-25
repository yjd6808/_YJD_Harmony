/*
 * 작성자: 윤정도
 * 생성일: 7/23/2023 4:27:54 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "FrameEventLoader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/Global.h>
#include <SteinsGate/Client/JsonUtilEx.h>

#include <JCore/FileSystem/Path.h>

USING_NS_JS;
USING_NS_JC;

FrameEventLoader::FrameEventLoader(DataManagerAbstract* manager, ActorType_t actorType)
	: ConfigFileLoaderAbstract(manager)
	, m_eActorType(actorType)
{}

bool FrameEventLoader::load() {
	Value root;

	if (!loadJson(root))
		return false;

	try {
		Value attackDataListRoot = root["frame_event"];

		for (int i = 0; i < attackDataListRoot.size(); ++i) {
			Value& frameEventRoot = attackDataListRoot[i];
			FrameEvent* pFrameEvent = nullptr;
			const FrameEventType_t eType = (FrameEventType_t)frameEventRoot["type"].asInt();
			const int iCode = frameEventRoot["code"].asInt();
			//const SGString Name = JsonUtilEx::getString(frameEventRoot["name"]);

			switch (eType) {
			case FrameEventType::Spawn: {
				const auto pSpawnEvent = dbg_new FrameEventSpawn;
				readFrameEventSpawn(frameEventRoot, pSpawnEvent);
				pFrameEvent = pSpawnEvent;
				break;
			}
			case FrameEventType::AttackBoxInstant: {
				const auto pAttackBoxInstantEvent = dbg_new FrameEventAttackBoxInstant;
				readFrameEventAttackBoxInstant(frameEventRoot, pAttackBoxInstantEvent);
				pFrameEvent = pAttackBoxInstantEvent;
				break;
			}
			}

			if (pFrameEvent == nullptr) {
				DebugAssert(pFrameEvent != nullptr);
				continue;
			}

			pFrameEvent->Code = iCode;
			pFrameEvent->Type = eType;

			addData(pFrameEvent);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

void FrameEventLoader::readFrameEventSpawn(Value& frameEventRoot, FrameEventSpawn* frameEvent) {
	frameEvent->SpawnType = (FrameEventSpawnType_t)frameEventRoot["spawn_type"].asInt();
	frameEvent->SpawnCode = (FrameEventSpawnType_t)frameEventRoot["spawn_code"].asInt();
}

void FrameEventLoader::readFrameEventAttackBoxInstant(Value& frameEventRoot, JCORE_OUT FrameEventAttackBoxInstant* frameInfo) {
	frameInfo->AttackDataCode = frameEventRoot["attack_data_code"].asInt();
	JsonUtilEx::parseActorRect(frameEventRoot["actor_rect"], frameInfo->Rect);
}
