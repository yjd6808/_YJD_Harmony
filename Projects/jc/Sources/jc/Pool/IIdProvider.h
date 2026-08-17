/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:00:00 PM
 * =====================
 * 키 발급기 인터페이스 (IProvider → IIdProvider 개명: 2026-08-16)
 *
 * [만든 이유]
 *  리소스 매니저처럼 "중복되지 않는 정수 키"가 필요한 곳에서
 *  키 발급 정책(단순 증가 / 반납 키 재사용 등)을 교체 가능하게 하기 위함이다.
 *  사용처는 이 인터페이스만 보관하고, 실제 정책은 Provider<T>로 감싸서 주입한다.
 *
 * [규약]
 *  - INVALID_KEY(0)는 "키 없음"을 뜻하는 예약값이다. 발급은 항상 1부터 시작한다.
 *  - Acquire로 받은 키는 더 이상 쓰지 않게 되면 Release로 반드시 반납한다.
 */

#pragma once

#include "jc/Type.h"

NS_JC_BEGIN

template <typename TKey>
struct JC_NOVTABLE IIdProvider
{
	static constexpr TKey INVALID_KEY = TKey(0);	// 무효 키. 발급은 1부터

	virtual ~IIdProvider() = default;

	// 새 키를 발급한다. 반환값은 항상 INVALID_KEY가 아니다.
	virtual TKey Acquire() = 0;

	// 발급받은 키를 반납한다. 정책에 따라 재사용될 수 있다.
	virtual void Release(TKey _key) = 0;

	// 현재 발급 중인(반납되지 않은) 키 수. 누수 진단용.
	virtual _sz GetAcquiredCount() const = 0;

	// 발급 상태를 초기 상태로 되돌린다. (모든 키 무효화)
	virtual void Reset() = 0;
};

NS_END
