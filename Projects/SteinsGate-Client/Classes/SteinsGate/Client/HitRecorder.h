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
	HitRecorder(Actor* _pOwner, Actor* _pRecorder);
	HitRecorder(Actor* _pOwner, Actor* _pRecorder, int _hitPossibleListSize, int _alreadyHitEnemySize);

	bool IsAlreadyHit(Actor* _pHitEnemy);
	void Record(const FrameEventAttackBoxInstant* _pFrameEvent);
	void Record(const ActorRect& _absoluteActorRect, int _attackDataCode);
	void ClearAlreadyHitEnemies();

	void SetOwner(Actor* _pOwner);
	void SetRecord(bool _enabled);
	void SetAlreadyHitRecord(bool _enabled);

	bool HasSingleHitCallback(int _definedEventId);
	bool AddSingleHitCallback(int _definedEventId, const SGHitSingleCallbackFn& _callback); // 한마리 한마리 호출
	bool AddSingleHitCallback(int _definedEventId, SGHitSingleCallbackFn&& _callback); // 한마리 한마리 호출
	bool RemoveSingleHitCallback(int _definedEventId);
	void ClearSingleHitCallback();

	bool HasMultiHitCallback(int _definedEventId);
	bool AddMultiHitCallback(int _definedEventId, const SGHitMultiCallbackFn& _callback); // 해당 프레임내에서 타격된 모든 몬스터 정보 호출
	bool AddMultiHitCallback(int _definedEventId, SGHitMultiCallbackFn&& _callback); // 해당 프레임내에서 타격된 모든 몬스터 정보 호출
	bool RemoveMultiHitCallback(int _definedEventId);
	void ClearMultiHitCallback();

private:
	/*
	 * 오너, 레코더에 대한 이해
	 * ex 1) 캐릭터가 쏜 총알이 날아가면서 부딫히는 적을 기록하는 경우 (오너: 캐릭터, 레코더: 총알)
	 * ex 2) 캐릭터에 부딫히는 적을 기록하는 경우 (오너: 캐릭터, 레코더: 캐릭터)
	 *
	 */

	Actor* pOwner_; // 레코더 소유자
	Actor* pRecorder_; // 히트박스 대상
	JCore::Event<HitInfo&> singleHitEvent_;
	JCore::Event<SGHitInfoList&, int> multiHitEvent_;
	SGVector<HitInfo> hitPossibleList_;
	SGHashMap<Actor*, Actor*> alreadyHitEnemy_;
	bool recordAlreadyHit_;
	bool record_;
};
