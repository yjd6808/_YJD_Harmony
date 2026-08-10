/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:00:00 PM
 * =====================
 * 키 발급기 어댑터
 *
 * [사용법]
 *  정책 타입을 템플릿 인자로 받아 IProvider로 노출한다.
 *
 *  Provider<AutoIncrementLinkedList<_u64>> provider;
 *  _u64 key = provider.Acquire();
 *  provider.Release(key);
 *
 *  // IProvider<_u64>* 로 주입 가능
 *  IProvider<_u64>* pProvider = &provider;
 *
 * [T 요구사항]
 *  T는 IProvider<TKey>를 구현한 정책 타입이어야 한다.
 *  (AutoIncrementSimple<_u64>, AutoIncrementLinkedList<_u64> 등)
 */

#pragma once

#include "jc/Pool/IProvider.h"
#include "jc/Pool/AutoIncrementSimple.h"
#include "jc/Pool/AutoIncrementLinkedList.h"

NS_JC_BEGIN

template <typename T>
class Provider final : public IProvider<decltype(T().Acquire())>
{
public:
	using TKeyType = decltype(T().Acquire());
	using TPolicy = T;

	Provider() = default;
	~Provider() override = default;

	virtual TKeyType Acquire() override { return policy_.Acquire(); }
	virtual void Release(TKeyType _key) override { policy_.Release(_key); }
	virtual _sz GetAcquiredCount() const override { return policy_.GetAcquiredCount(); }
	virtual void Reset() override { policy_.Reset(); }

	// 정책 직접 접근 (정책 고유 기능이 필요할 때: GetFreeCount 등)
	TPolicy& GetPolicy() { return policy_; }
	const TPolicy& GetPolicy() const { return policy_; }

private:
	TPolicy policy_;
};

NS_END
