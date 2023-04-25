﻿/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 6:16:33 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/Struct.h>

class ActorPartAnimation;
class Actor;

// 타격받은 대상 정보를 기록하는 용도
using SGHitInfoList = SGVector<SGHitInfo>;
using SGHitSingleCallbackFn = SGActionFn<SGHitInfo&>;
using SGHitMultiCallbackFn = SGActionFn<SGHitInfoList&, int>;

class HitRecorder
{
public:
	HitRecorder(Actor* recorder);
	HitRecorder(Actor* recorder, int hitPossibleListSize, int alreadyHitEnemySize);

	bool isAlreadyHit(Actor* hitEnemy);
	void record(ActorPartAnimation* animation);
	void record(const SGActorRect& absoluteActorRect, int attackDataCode);
	void clear();

	void setRecord(bool enabled);
	void setAlreadyHitRecord(bool enabled);
	void setSingleHitCallback(const SGHitSingleCallbackFn& callback);		// 한마리 한마리 호출
	void setMultiHitCallback(const SGHitMultiCallbackFn& callback);			// 해당 프레임내에서 타격된 모든 몬스터 정보 호출
private:
	Actor* m_pRecorder;
	SGHitSingleCallbackFn m_fnHitSingleCallback;
	SGHitMultiCallbackFn m_fnHitMultiCallback;
	SGVector<SGHitInfo> m_vHitPossibleList;
	SGHashMap<Actor*, Actor*> m_hAlreadyHitEnemy;
	bool m_bRecordAlreadyHit;
	bool m_bRecord;
};