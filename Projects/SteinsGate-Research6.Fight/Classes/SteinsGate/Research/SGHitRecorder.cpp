/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 6:17:15 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGHitRecorder.h"

#include <SteinsGate/Research/SGActor.h>
#include <SteinsGate/Research/SGMapLayer.h>
#include <SteinsGate/Research/SGDataManager.h>

SGHitRecorder::SGHitRecorder(SGActor* recorder)
	: m_pRecorder(recorder)
	, m_vHitPossibleList(16)
	, m_hAlreadyHitEnemy(32)
	, m_bRecord(true)
	, m_bRecordAlreadyHit(false)
{}

SGHitRecorder::SGHitRecorder(SGActor* recorder, int hitPossibleListSize, int alreadyHitEnemySize)
	: m_pRecorder(recorder)
	, m_vHitPossibleList(hitPossibleListSize)
	, m_hAlreadyHitEnemy(alreadyHitEnemySize)
	, m_bRecord(true)
	, m_bRecordAlreadyHit(false)
{}

bool SGHitRecorder::isAlreadyHit(SGActor* hitEnemy) {
	return m_hAlreadyHitEnemy.Exist(hitEnemy);
}

void SGHitRecorder::record(SGActorPartAnimation* animation) {

	if (!m_bRecord)
		return;

	SGFrameInfo* pFrameInfo = animation->getRunningFrameInfo();

	if (pFrameInfo->FrameEvent != FrameEventType::AttackBoxInstant) {
		return;
	}

	SGFrameInfoAttackBoxInstant* pAttackBoxInstantInfo = (SGFrameInfoAttackBoxInstant*)pFrameInfo;

	// 절대 위치 박스로 변환
	SGActorRect absoluteActorRect = SGActor::convertAbsoluteActorRect(m_pRecorder, pAttackBoxInstantInfo->Rect);	
	record(absoluteActorRect, pAttackBoxInstantInfo->FrameEventId);
}

void SGHitRecorder::record(const SGActorRect& absoluteActorRect, int attackDataCode) {
	SGMapLayer* pMapLayer = m_pRecorder->getMapLayer();

	int iNewHitCount = 0;
	m_vHitPossibleList.Clear();

	pMapLayer->collectEnemiesInActorRect(m_pRecorder, absoluteActorRect, m_vHitPossibleList);

	for (int i = 0; i < m_vHitPossibleList.Size(); ++i) {
		SGHitInfo& hitInfo = m_vHitPossibleList[i];
		hitInfo.AttackDataInfo = SGDataManager::get()->getAttackDataInfo(attackDataCode);
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

void SGHitRecorder::clear() {
	m_hAlreadyHitEnemy.Clear();
}

void SGHitRecorder::setRecord(bool enabled) {
	m_bRecord = enabled;
}

void SGHitRecorder::setAlreadyHitRecord(bool enabled) {
	m_bRecordAlreadyHit = enabled;
}

void SGHitRecorder::setSingleHitCallback(const SGHitSingleCallbackFn& callback) {
	m_fnHitSingleCallback = callback;
}

void SGHitRecorder::setMultiHitCallback(const SGHitMultiCallbackFn& callback) {
	m_fnHitMultiCallback = callback;
}
