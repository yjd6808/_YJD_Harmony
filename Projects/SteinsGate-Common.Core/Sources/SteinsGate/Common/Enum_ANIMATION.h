/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 8:04:10 PM
 * =====================
 *
 */


#pragma once

JCORE_SENUM_BEGIN(FrameEventType)
None,
Spawn,
AttackBoxInstant,
Max
JCORE_SENUM_END(FrameEventType)

// ==========================================================================================

JCORE_SENUM_BEGIN(FrameEventSpawnType)
Projectile,
AttackBox,
Max
JCORE_SENUM_END(FrameEventSpawnType)

// ==========================================================================================

/*
 * TODO: ActorSpriteData가 아닌 ActorPartSpriteData의 멤버로 오는게 올바르긴하다. 이 TODO는 JsonUtilEx::parseActorSpriteData의 TODO를 처리할 때 알아서 해결됨.
 *
 * 프레임내부에서 스프라이트 위치지정을 어떻게 할지 정하는 규칙
 * 기존 리소스의 이미지를 유심히 관찰 후 스프라이트 위치를 어떻게 적용시킬지 규칙을 정하였다.
 *
 * [InFrameSize]
 * 기존 스프라이트 프레임의 위치를 리소스에서 정해진 위치 그대로 적용
 *  1. 캐릭터 모션같이 프레임 내부에서 스프라이트 위치가 고정되어 있는 경우
 *  2. 몬스터의 각 파츠별 위치가 프레임 내부에서 고정되어 있는 경우
 *
 * [InCustomFrameSize]
 * 리소스에 기입된 프레임 사이즈를 무시하고 내가 자체적으로 프레임사이즈를 정의한다.
 *
 *  1. 맵 오브젝트(게이트, 장애물)의 경우
 * [InIgnoredFrameSize]
 * 프레임없이 
 */ 
JCORE_SENUM_BEGIN(ActorPartSpritePositioningRule)
InFrameSize,		// 캐릭터 모션
InCustomFrameSize,	// 맵 오브젝트(게이트, 장애물)
InIgnoredFrameSize	// 프로젝틸
JCORE_SENUM_END(ActorPartSpritePositioningRule)


