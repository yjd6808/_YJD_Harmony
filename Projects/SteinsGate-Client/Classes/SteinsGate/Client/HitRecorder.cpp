/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 6:17:15 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "HitRecorder.h"

#include <SteinsGate/Client/Actor.h>
#include <SteinsGate/Client/MapLayer.h>
#include <SteinsGate/Client/DataManager.h>

//////////////////////////////////////////////////////////////////////////////////////////
HitRecorder::HitRecorder(Actor* _pOwner, Actor* _pRecorder)
: owner_(_pOwner)
, recorder_(_pRecorder)
, hitPossibleList_(16)
, alreadyHitEnemy_(32)
, recordAlreadyHit_(false)
, record_(true)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
HitRecorder::HitRecorder(Actor* _pOwner, Actor* _pRecorder, int _hitPossibleListSize, int _alreadyHitEnemySize)
: owner_(_pOwner)
, recorder_(_pRecorder)
, hitPossibleList_(_hitPossibleListSize)
, alreadyHitEnemy_(_alreadyHitEnemySize)
, recordAlreadyHit_(false)
, record_(true)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::isAlreadyHit(Actor* _pHitEnemy)
{
	return alreadyHitEnemy_.Exist(_pHitEnemy);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::record(const FrameEventAttackBoxInstant* _pFrameEvent)
{
	if (!record_)
		return;

	// 절대 위치 박스로 변환
	ActorRect absoluteActorRect = Actor::convertAbsoluteActorRect(recorder_, _pFrameEvent->rect_);
	record(absoluteActorRect, _pFrameEvent->attackDataCode_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::record(const ActorRect& _absoluteActorRect, int _attackDataCode)
{
	MapLayer* pMapLayer = recorder_->getMapLayer();

	int newHitCount = 0;
	hitPossibleList_.Clear();

	pMapLayer->collectEnemiesInActorRect(recorder_, _absoluteActorRect, hitPossibleList_);

	for (int i = 0; i < hitPossibleList_.Size(); ++i)
	{
		HitInfo& hitInfo = hitPossibleList_[i];
		hitInfo.AttackDataInfo = DataManager::Get()->getAttackDataInfo(owner_->getType(), _attackDataCode);
		hitInfo.Attacker = recorder_;

		singleHitEvent_.Invoke(hitInfo);

		if (recordAlreadyHit_ && alreadyHitEnemy_.Insert(hitInfo.HitTarget, hitInfo.HitTarget))
		{
			++newHitCount;
		}
	}

	multiHitEvent_.Invoke(hitPossibleList_, newHitCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::clearAlreadyHitEnemies()
{
	alreadyHitEnemy_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::setOwner(Actor* _pOwner)
{
	owner_ = _pOwner;
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::setRecord(bool _enabled)
{
	record_ = _enabled;
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::setAlreadyHitRecord(bool _enabled)
{
	recordAlreadyHit_ = _enabled;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::hasSingleHitCallback(int _definedEventId)
{
	return singleHitEvent_.IsRegistered(_definedEventId);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::addSingleHitCallback(int _definedEventId, const SGHitSingleCallbackFn& _callback)
{
	return singleHitEvent_.Register(_definedEventId, _callback);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::addSingleHitCallback(int _definedEventId, SGHitSingleCallbackFn&& _callback)
{
	return singleHitEvent_.Register(_definedEventId, JCore::Move(_callback));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::removeSingleHitCallback(int _definedEventId)
{
	return singleHitEvent_.Unregister(_definedEventId);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::clearSingleHitCallback()
{
	singleHitEvent_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::hasMultiHitCallback(int _definedEventId)
{
	return multiHitEvent_.IsRegistered(_definedEventId);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::addMultiHitCallback(int _definedEventId, const SGHitMultiCallbackFn& _callback)
{
	return multiHitEvent_.Register(_definedEventId, _callback);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::addMultiHitCallback(int _definedEventId, SGHitMultiCallbackFn&& _callback)
{
	return multiHitEvent_.Register(_definedEventId, Move(_callback));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HitRecorder::removeMultiHitCallback(int _definedEventId)
{
	return multiHitEvent_.Unregister(_definedEventId);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitRecorder::clearMultiHitCallback()
{
	multiHitEvent_.Clear();
}
