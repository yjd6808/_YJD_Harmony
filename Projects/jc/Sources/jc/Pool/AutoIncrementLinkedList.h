/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:00:00 PM
 * =====================
 * 연결리스트(freeList_) 기반 재사용 키 발급 정책
 *
 * [동작]
 *  - 기본은 1, 2, 3, ... 단순 증가 발급.
 *  - Release된 키는 freeList_ 머리에 보관했다가(O(1))
 *    다음 Acquire 때 머리에서 꺼내 재발급한다. (LIFO)
 *
 * [언제 쓰나?]
 *  - 생성/삭제가 잦은 리소스 키처럼 키 공간을 조밀하게 유지하고 싶을 때
 *  - ResourceMgr의 기본 발급 정책이다. (sgf D-12)
 *
 * [주의]
 *  - 재사용 정책이므로 "예전에 쓰던 키를 넌가 들고 있으면" 다른 리소스를
 *    가리킬 수 있다. 키를 오래 보관하는 쪽은 사용 전 유효성 확인이 필요하다.
 */

#pragma once

#include "jc/Assert.h"
#include "jc/Container/LinkedList.h"
#include "jc/Pool/IProvider.h"

NS_JC_BEGIN

template <typename TKey>
class AutoIncrementLinkedList final : public IProvider<TKey>
{
	using TProvider = IProvider<TKey>;

public:
	AutoIncrementLinkedList()
		: next_(TProvider::INVALID_KEY + 1)
		, acquiredCount_(0)
	{
	}

	~AutoIncrementLinkedList() override = default;

	virtual TKey Acquire() override
	{
		acquiredCount_ += 1;

		// 반납된 키가 있으면 머리에서 꺼내 재발급 (LIFO)
		if (!freeList_.IsEmpty())
		{
			TKey reused;
			freeList_.PopFront(&reused);
			return reused;
		}

		return next_++;
	}

	virtual void Release(TKey _key) override
	{
		jc_assert_msg(_key != TProvider::INVALID_KEY, "무효 키는 반납할 수 없습니다.");
		jc_assert_msg(_key < next_, "발급한 적 없는 키입니다.");
		jc_assert_msg(acquiredCount_ > 0, "발급 중인 키가 없는데 반납했습니다.");

		acquiredCount_ -= 1;
		freeList_.PushFront(_key);	// 머리 삽입 O(1)
	}

	virtual _sz GetAcquiredCount() const override
	{
		return acquiredCount_;
	}

	virtual void Reset() override
	{
		next_ = TProvider::INVALID_KEY + 1;
		acquiredCount_ = 0;
		freeList_.Clear();
	}

	// 재사용 대기 중인 키 수 (진단용)
	_sz GetFreeCount() const { return static_cast<_sz>(freeList_.Size()); }

	// 다음에 "새로" 발급될 키 미리보기 (재사용 키가 있으면 그쪽이 먼저다)
	TKey GetNextKey() const { return next_; }

private:
	TKey next_;					// 다음에 새로 발급할 키
	_sz acquiredCount_;			// 발급 중인 키 수
	LinkedList<TKey> freeList_;	// 반납된 키 보관소 (머리 삽입/꺼내기)
};

NS_END
