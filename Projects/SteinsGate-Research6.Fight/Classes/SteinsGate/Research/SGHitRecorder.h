/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 6:16:33 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGStruct.h>

class SGActorPartAnimation;
class SGActor;

// 타격받은 대상 정보를 기록하는 용도
using SGHitInfoList = SGVector<SGHitInfo>;
using SGHitSingleCallbackFn = SGActionFn<SGHitInfo&>;
using SGHitMultiCallbackFn = SGActionFn<SGHitInfoList&, int>;


class SGHitRecorder
{
public:
	SGHitRecorder(SGPhysicsActor* recorder);
	SGHitRecorder(SGPhysicsActor* recorder, int hitPossibleListSize, int alreadyHitEnemySize);

	bool isAlreadyHit(SGActor* hitEnemy);
	void record(SGActorPartAnimation* animation);
	void clear();

	void setRecord(bool enabled);
	void setAlreadyHitRecord(bool enabled);
	void setSingleHitCallback(const SGHitSingleCallbackFn& callback);		// 한마리 한마리 호출
	void setMultiHitCallback(const SGHitMultiCallbackFn& callback);			// 해당 프레임내에서 타격된 모든 몬스터 정보 호출
private:
	SGPhysicsActor* m_pRecorder;
	SGHitSingleCallbackFn m_fnHitSingleCallback;
	SGHitMultiCallbackFn m_fnHitMultiCallback;
	SGVector<SGHitInfo> m_vHitPossibleList;
	SGHashMap<SGActor*, SGActor*> m_hAlreadyHitEnemy;
	bool m_bRecordAlreadyHit;
	bool m_bRecord;
};
