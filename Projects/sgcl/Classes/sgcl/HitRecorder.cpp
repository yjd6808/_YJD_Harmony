/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 6:17:15 PM
 * =====================
 *
 */

#include "Core.h"
#include "HitRecorder.h"

#include <sg/_Util/DescLoaderMgr.h>

#include <sgcl/Actor.h>
#include <sgcl/_Layer/Layer_Map.h>

//////////////////////////////////////////////////////////////////////////////////////////
HitRecorder::HitRecorder(Actor* _pOwner, Actor* _pRecorder)
: pOwner_(_pOwner)
, pRecorder_(_pRecorder)
, hitPossibleList_(16)
, alreadyHitEnemy_(32)
, recordAlreadyHit_(false)
, record_(true)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
HitRecorder::HitRecorder(Actor* _pOwner, Actor* _pRecorder, int _hitPossibleListSize, int _alreadyHitEnemySize)
: pOwner_(_pOwner)
, pRecorder_(_pRecorder)
, hitPossibleList_(_hitPossibleListSize)
, alreadyHitEnemy_(_alreadyHitEnemySize)
, recordAlreadyHit_(false)
, record_(true)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::IsAlreadyHit(Actor* _pHitEnemy)
{
	return alreadyHitEnemy_.Exist(_pHitEnemy);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::Record(const FrameEventAttackBoxInstant* _pFrameEvent)
{
	if (!record_)
		return;

	// 절대 위치 박스로 변환
	ActorRect absoluteActorRect = Actor::ConvertAbsoluteActorRect(pRecorder_, _pFrameEvent->rect_);
	Record(absoluteActorRect, _pFrameEvent->attackDataCode_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::Record(const ActorRect& _absoluteActorRect, int _attackDataCode)
{
	MapLayer* pMapLayer = pRecorder_->GetMapLayer();

	int newHitCount = 0;
	hitPossibleList_.Clear();

	pMapLayer->CollectEnemiesInActorRect(pRecorder_, _absoluteActorRect, hitPossibleList_);

	for (int i = 0; i < hitPossibleList_.Size(); ++i)
	{
		HitInfo& hitInfo = hitPossibleList_[i];
		hitInfo.pAttackDataInfo_ = g_cDescMgr.GetAttackDataInfo(pOwner_->GetType(), _attackDataCode);
		hitInfo.pAttacker_ = pRecorder_;

		singleHitEvent_.Invoke(hitInfo);

		if (recordAlreadyHit_ && alreadyHitEnemy_.Insert(hitInfo.pHitTarget_, hitInfo.pHitTarget_))
		{
			++newHitCount;
		}
	}

	multiHitEvent_.Invoke(hitPossibleList_, newHitCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::ClearAlreadyHitEnemies()
{
	alreadyHitEnemy_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::SetOwner(Actor* _pOwner)
{
	pOwner_ = _pOwner;
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::SetRecord(bool _enabled)
{
	record_ = _enabled;
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::SetAlreadyHitRecord(bool _enabled)
{
	recordAlreadyHit_ = _enabled;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::HasSingleHitCallback(int _definedEventId)
{
	return singleHitEvent_.IsRegistered(_definedEventId);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::AddSingleHitCallback(int _definedEventId, const SGHitSingleCallbackFn& _callback)
{
	return singleHitEvent_.Register(_definedEventId, _callback);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::AddSingleHitCallback(int _definedEventId, SGHitSingleCallbackFn&& _callback)
{
	return singleHitEvent_.Register(_definedEventId, jc::Move(_callback));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::RemoveSingleHitCallback(int _definedEventId)
{
	return singleHitEvent_.Unregister(_definedEventId);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::ClearSingleHitCallback()
{
	singleHitEvent_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::HasMultiHitCallback(int _definedEventId)
{
	return multiHitEvent_.IsRegistered(_definedEventId);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::AddMultiHitCallback(int _definedEventId, const SGHitMultiCallbackFn& _callback)
{
	return multiHitEvent_.Register(_definedEventId, _callback);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::AddMultiHitCallback(int _definedEventId, SGHitMultiCallbackFn&& _callback)
{
	return multiHitEvent_.Register(_definedEventId, Move(_callback));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::RemoveMultiHitCallback(int _definedEventId)
{
	return multiHitEvent_.Unregister(_definedEventId);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::ClearMultiHitCallback()
{
	multiHitEvent_.Clear();
}
