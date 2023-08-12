/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 6:16:33 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>
#include <SteinsGate/Client/FrameEvent.h>

class ActorPartAnimation;
class Actor;

// 타격받은 대상 정보를 기록하는 용도
using SGHitInfoList = SGVector<HitInfo>;
using SGHitSingleCallbackFn = SGActionFn<HitInfo&>;
using SGHitMultiCallbackFn = SGActionFn<SGHitInfoList&, int>;

class HitRecorder
{
public:
	HitRecorder(Actor* owner, Actor* recorder);
	HitRecorder(Actor* owner, Actor* recorder, int hitPossibleListSize, int alreadyHitEnemySize);

	bool isAlreadyHit(Actor* hitEnemy);
	void record(const FrameEventAttackBoxInstant* frameEvent);
	void record(const ActorRect& absoluteActorRect, int attackDataCode);
	void clearAlreadyHitEnemies();

	void setOwner(Actor* owner);
	void setRecord(bool enabled);
	void setAlreadyHitRecord(bool enabled);

	bool hasSingleHitCallback(int definedEventId);
	bool addSingleHitCallback(int definedEventId, const SGHitSingleCallbackFn& callback);		// 한마리 한마리 호출
	bool addSingleHitCallback(int definedEventId, SGHitSingleCallbackFn&& callback);			// 한마리 한마리 호출
	bool removeSingleHitCallback(int definedEventId);
	void clearSingleHitCallback();

	bool hasMultiHitCallback(int definedEventId);
	bool addMultiHitCallback(int definedEventId, const SGHitMultiCallbackFn& callback);		// 해당 프레임내에서 타격된 모든 몬스터 정보 호출
	bool addMultiHitCallback(int definedEventId, SGHitMultiCallbackFn&& callback);			// 해당 프레임내에서 타격된 모든 몬스터 정보 호출
	bool removeMultiHitCallback(int definedEventId);
	void clearMultiHitCallback();

private:

	/*
	 * 오너, 레코더에 대한 이해
	 * ex 1) 캐릭터가 쏜 총알이 날아가면서 부딫히는 적을 기록하는 경우 (오너: 캐릭터, 레코더: 총알)
	 * ex 2) 캐릭터에 부딫히는 적을 기록하는 경우 (오너: 캐릭터, 레코더: 캐릭터)
	 * 
	 */

	Actor* m_pOwner;		// 레코더 소유자
	Actor* m_pRecorder;		// 히트박스 대상
	JCore::Event<HitInfo&> m_SingleHitEvent;
	JCore::Event<SGHitInfoList&, int> m_MultiHitEvent;
	SGVector<HitInfo> m_vHitPossibleList;
	SGHashMap<Actor*, Actor*> m_hAlreadyHitEnemy;
	bool m_bRecordAlreadyHit;
	bool m_bRecord;
};
