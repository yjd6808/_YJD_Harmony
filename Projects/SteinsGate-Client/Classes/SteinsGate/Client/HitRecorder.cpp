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

HitRecorder::HitRecorder(Actor* owner, Actor* recorder)
	: m_pOwner(owner)
	, m_pRecorder(recorder)
	, m_vHitPossibleList(16)
	, m_hAlreadyHitEnemy(32)
	, m_bRecordAlreadyHit(false)
	, m_bRecord(true)
{}

HitRecorder::HitRecorder(Actor* owner, Actor* recorder, int hitPossibleListSize, int alreadyHitEnemySize)
	: m_pOwner(owner)
	, m_pRecorder(recorder)
	, m_vHitPossibleList(hitPossibleListSize)
	, m_hAlreadyHitEnemy(alreadyHitEnemySize)
	, m_bRecordAlreadyHit(false)
	, m_bRecord(true)
{}

bool HitRecorder::isAlreadyHit(Actor* hitEnemy) {
	return m_hAlreadyHitEnemy.Exist(hitEnemy);
}

void HitRecorder::record(const FrameEventAttackBoxInstant* frameEvent) {

	if (!m_bRecord)
		return;

	// 절대 위치 박스로 변환
	ActorRect absoluteActorRect = Actor::convertAbsoluteActorRect(m_pRecorder, frameEvent->Rect);
	record(absoluteActorRect, frameEvent->AttackDataCode);
}

void HitRecorder::record(const ActorRect& absoluteActorRect, int attackDataCode) {
	MapLayer* pMapLayer = m_pRecorder->getMapLayer();

	int iNewHitCount = 0;
	m_vHitPossibleList.Clear();

	pMapLayer->collectEnemiesInActorRect(m_pRecorder, absoluteActorRect, m_vHitPossibleList);

	for (int i = 0; i < m_vHitPossibleList.Size(); ++i) {
		HitInfo& hitInfo = m_vHitPossibleList[i];
		hitInfo.AttackDataInfo = DataManager::Get()->getAttackDataInfo(m_pOwner->getType(), attackDataCode);
		hitInfo.Attacker = m_pRecorder;

		if (m_fnHitSingleCallback)
			m_fnHitSingleCallback(hitInfo);

		if (m_bRecordAlreadyHit && m_hAlreadyHitEnemy.Insert(hitInfo.HitTarget, hitInfo.HitTarget)) {
			++iNewHitCount;
		}
	}

	if (m_fnHitMultiCallback)
		m_fnHitMultiCallback(m_vHitPossibleList, iNewHitCount);
}

void HitRecorder::clear() {
	m_hAlreadyHitEnemy.Clear();
}

void HitRecorder::setOwner(Actor* owner) {
	m_pOwner = owner;
}

void HitRecorder::setRecord(bool enabled) {
	m_bRecord = enabled;
}

void HitRecorder::setAlreadyHitRecord(bool enabled) {
	m_bRecordAlreadyHit = enabled;
}

void HitRecorder::setSingleHitCallback(const SGHitSingleCallbackFn& callback) {
	m_fnHitSingleCallback = callback;
}

void HitRecorder::setMultiHitCallback(const SGHitMultiCallbackFn& callback) {
	m_fnHitMultiCallback = callback;
}
