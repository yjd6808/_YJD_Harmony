/*
 * 작성자: 윤정도
 * 생성일: 7/23/2023 4:27:54 AM
 * =====================
 *
 */


#pragma once

#include <sg/_Util/DescBase.h>

struct FrameEvent : SDescBase
{
	~FrameEvent() override = default;

	FrameEventType_t type_;
};

struct FrameEventSpawn : FrameEvent
{
	~FrameEventSpawn() override = default;

	FrameEventSpawnType_t spawnType_;
	int spawnCode_;
};

struct FrameEventAttackBoxInstant : FrameEvent
{
	~FrameEventAttackBoxInstant() override = default;

	int attackDataCode_;
	ActorRect rect_;
};
