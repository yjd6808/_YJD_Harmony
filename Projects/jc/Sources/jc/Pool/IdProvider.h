/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:00:00 PM
 * 수정일: 8/16/2026 (AutoIncrementSimple → IdProvider 개명: 2026-08-16)
 * =====================
 * 순수 증가 키 발급 정책 (freeList 없음)
 *
 * [동작]
 *  - Acquire마다 1, 2, 3, ... 순서로 발급한다.
 *  - Release된 키는 재사용하지 않는다. (버려짐)
 *
 * [언제 쓰나?]
 *  - 키 고갈 걸정이 없는 경우(_u64는 초당 1억 발급해도 약 5,849년)
 *  - "한 번 쓴 키는 영원히 다시 등장하지 않는다"는 보장이 필요할 때
 *    (달링 포인터 감지가 쉬워 디버깅에 유리하다)
 *  - GameObject GID 전용 정책. (sgf v2)
 */

#pragma once

#include "jc/Assert.h"
#include "jc/Pool/IIdProvider.h"

NS_JC_BEGIN

template <typename TKey>
class IdProvider final : public IIdProvider<TKey>
{
	using TProvider = IIdProvider<TKey>;

public:
	IdProvider()
		: next_(TProvider::INVALID_KEY + 1)
		, acquiredCount_(0)
	{
	}

	~IdProvider() override = default;

	virtual TKey Acquire() override
	{
		acquiredCount_ += 1;
		return next_++;
	}

	virtual void Release(TKey _key) override
	{
		jc_assert_msg(_key != TProvider::INVALID_KEY, "무효 키는 반납할 수 없습니다.");
		jc_assert_msg(_key < next_, "발급한 적 없는 키입니다.");
		jc_assert_msg(acquiredCount_ > 0, "발급 중인 키가 없는데 반납했습니다.");

		acquiredCount_ -= 1;
	}

	virtual _sz GetAcquiredCount() const override
	{
		return acquiredCount_;
	}

	virtual void Reset() override
	{
		next_ = TProvider::INVALID_KEY + 1;
		acquiredCount_ = 0;
	}

	// 다음에 발급될 키 미리보기 (진단용)
	TKey GetNextKey() const { return next_; }

private:
	TKey next_;				// 다음에 발급할 키
	_sz acquiredCount_;		// 발급 중인 키 수
};

NS_END
