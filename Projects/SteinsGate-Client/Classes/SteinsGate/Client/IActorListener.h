/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:33:33 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

class Actor;
class ActorPartAnimation;
class FrameTexture;
class HostPlayer;
class HitRecorder;
class AIActivity;

class JCORE_NOVTABLE IActorListener
{
public:
	enum Type
	{
		eCharacter,
		eMonster,
		eProjectile,
		eHost
	};

	virtual ~IActorListener() = default;

	virtual void onCreated() {}			// 액터 박스에서 생성되어 맵 레이어에 등록될 때마다 호출
	virtual void onCleanUp() {}			// 엑터 박스에서 제거되어 맵 레이어에서 제거되어 더이상 사용되지 않는 경우 (풀에 돌아간 경우도 포함)
	virtual void onUpdate(float dt) {}
	virtual void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture)	{}
	virtual void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {}
	virtual void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {}
	virtual void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {}

	// TODO: 아래 마우스 관련 기능도 구현
	virtual void onMouseEnter() {}
	virtual void onMouseMove() {}
	virtual void onMouseEnd() {}

	// AI 컴포넌트가 부착된 경우
	virtual void onActivitiyBegin(AIActivity* activity) {}
	virtual void onActivitiyEnd(AIActivity* activity) {}

	virtual void onHit(const HitInfo& info) {}

	virtual Type getListenerType() const = 0;
};


