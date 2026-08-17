/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:00:00 PM
 * 수정일: 8/16/2026 (IProvider → IIdProvider / AutoIncrement* → IdProvider* 개명 반영)
 * =====================
 * 키 발급기 어댑터
 *
 * [사용법]
 *  정책 타입을 템플릿 인자로 받아 IIdProvider로 노출한다.
 *
 *  Provider<IdProviderReuse<_u64>> provider;
 *  _u64 key = provider.Acquire();
 *  provider.Release(key);
 *
 *  // IIdProvider<_u64>* 로 주입 가능
 *  IIdProvider<_u64>* pProvider = &provider;
 *
 * [T 요구사항]
 *  T는 IIdProvider<TKey>를 구현한 정책 타입이어야 한다.
 *  (IdProvider<_u64>, IdProviderReuse<_u64> 등)
 */

#pragma once

#include "jc/Pool/IIdProvider.h"
#include "jc/Pool/IdProvider.h"
#include "jc/Pool/IdProviderReuse.h"

NS_JC_BEGIN

template <typename T>
class Provider final : public IIdProvider<decltype(T().Acquire())>
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
