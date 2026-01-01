/*
 * 작성자: 윤정도
 * 생성일: 7/23/2023 4:27:54 AM
 * =====================
 *
 */

#include "Core.h"
#include "FrameEventLoader.h"

#include <sgcl/ImagePackManager.h>
#include <sgcl/Global.h>
#include <sgcl/JsonUtilEx.h>

#include <jc/FileSystem/Path.h>

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
FrameEventLoader::FrameEventLoader(DescLoaderMgr* _pManager, ActorType_t _actorType)
: DescLoaderAbstract(_pManager)
, actorType_(_actorType)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool FrameEventLoader::Load()
{
	Value root;

	if (!LoadJson(root))
		return false;

	try
	{
		Value frameEventListRoot = root["frame_event"];

		for (size_t i = 0; i < frameEventListRoot.size(); ++i)
		{
			Value& frameEventRoot = frameEventListRoot[(ArrayIndex)i];
			FrameEvent* pFrameEvent = nullptr;
			const FrameEventType_t frameEventType = (FrameEventType_t)frameEventRoot["type"].asInt();
			const int frameEventCode = frameEventRoot["code"].asInt();
			//const jc::String name = JsonUtilEx::getString(frameEventRoot["name"]);

			switch (frameEventType)
			{
			case FrameEventType::Spawn:
				{
					const auto pSpawnEvent = dbg_new FrameEventSpawn;
					ReadFrameEventSpawn(frameEventRoot, pSpawnEvent);
					pFrameEvent = pSpawnEvent;
					break;
				}
			case FrameEventType::AttackBoxInstant:
				{
					const auto pAttackBoxInstantEvent = dbg_new FrameEventAttackBoxInstant;
					ReadFrameEventAttackBoxInstant(frameEventRoot, pAttackBoxInstantEvent);
					pFrameEvent = pAttackBoxInstantEvent;
					break;
				}
			default:
				break;
			}

			if (pFrameEvent == nullptr)
			{
				jc_assert(pFrameEvent != nullptr);
				continue;
			}

			pFrameEvent->code_ = frameEventCode;
			pFrameEvent->type_ = frameEventType;

			AddData(pFrameEvent);
		}
	}
	catch (std::exception& exception)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), exception.what());
		return false;
	}

	Loaded();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void FrameEventLoader::ReadFrameEventSpawn(Value& _frameEventRoot, FrameEventSpawn* _pFrameEvent)
{
	_pFrameEvent->spawnType_ = (FrameEventSpawnType_t)_frameEventRoot["spawn_type"].asInt();
	_pFrameEvent->spawnCode_ = (FrameEventSpawnType_t)_frameEventRoot["spawn_code"].asInt();
}

//////////////////////////////////////////////////////////////////////////////////////////
void FrameEventLoader::ReadFrameEventAttackBoxInstant(Value& _frameEventRoot,
                                                      OUT FrameEventAttackBoxInstant* _pFrameEvent)
{
	_pFrameEvent->attackDataCode_ = _frameEventRoot["attack_data_code"].asInt();
	JsonUtilEx::ParseActorRect(_frameEventRoot["actor_rect"], _pFrameEvent->rect_);
}
