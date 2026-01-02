/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:33:33 AM
 * =====================
 *
 */

#pragma once


#include <sgcl/Core.h>

class Actor;
class ActorPartAnimation;
class FrameTexture;
class HostPlayer;
class HitRecorder;
class AIActivity;

class JC_NOVTABLE IActorListener
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

	virtual void OnCreated()
	{
	} // 액터 박스에서 생성되어 맵 레이어에 등록될 때마다 호출
	virtual void OnCleanUp()
	{
	} // 엑터 박스에서 제거되어 맵 레이어에서 제거되어 더이상 사용되지 않는 경우 (풀에 돌아간 경우도 포함)
	virtual void OnUpdate(float _dt)
	{
	}

	virtual void OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
	{
	}

	virtual void OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
	{
	}

	virtual void OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
	{
	}

	virtual void OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
	{
	}

	// TODO: 아래 마우스 관련 기능도 구현
	virtual void OnMouseEnter()
	{
	}

	virtual void OnMouseMove()
	{
	}

	virtual void OnMouseEnd()
	{
	}

	// AI 컴포넌트가 부착된 경우
	virtual void OnActivitiyBegin(AIActivity* _pActivity)
	{
	}

	virtual void OnActivitiyEnd(AIActivity* _pActivity)
	{
	}

	virtual void OnHit(const HitInfo& _info)
	{
	}

	virtual Type GetListenerType() const = 0;
};
