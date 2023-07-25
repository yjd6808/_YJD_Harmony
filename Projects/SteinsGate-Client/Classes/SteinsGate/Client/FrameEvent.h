/*
 * 작성자: 윤정도
 * 생성일: 7/23/2023 4:27:54 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>
#include <SteinsGate/Client/Struct.h>

struct FrameEvent : ConfigDataAbstract
{
	~FrameEvent() override = default;

	FrameEventType_t Type;
};

struct FrameEventSpawn : FrameEvent
{
	~FrameEventSpawn() override = default;

	FrameEventSpawnType_t SpawnType;
	int SpawnCode;
};

struct FrameEventAttackBoxInstant : FrameEvent
{
	~FrameEventAttackBoxInstant() override = default;

	int AttackDataCode;
	ActorRect Rect;
};


