/*
 * 작성자: 윤정도
 * 생성일: 7/30/2023 11:41:41 PM
 * =====================
 *
 */


#pragma once

#include <jc/TypeTraits.h>

NS_QRY_BEGIN
template <typename T>
struct QryHelper
{
	using THelper = QryHelper<T>;

	inline static thread_local bool IsSuccess;
	inline static thread_local int LastErrorCode;

	static void SetLastErrorCode(int _code)
	{
		LastErrorCode = _code;
		IsSuccess = _code == 0;
	}
};

NS_DETAIL_BEGIN
template <typename T>
struct IsQryHelper : jc::FalseType
{
};

template <template <typename> typename Base, typename U>
struct IsQryHelper<Base<U>> : jc::Conditional_t<jc::IsSameType_v<Base<U>, QryHelper<U>>, jc::TrueType, jc::FalseType>
{
};

NS_DETAIL_END

template <typename THelper>
static constexpr bool IsQryHelper_v = detail::IsQryHelper<THelper>::VALUE;

NS_QRY_END
