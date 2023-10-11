/*
 * 작성자: 윤정도
 * 생성일: 7/30/2023 11:41:41 PM
 * =====================
 *
 */


#pragma once

#include <JCore/TypeTraits.h>

#include <TF/Server/Namespace.h>

NS_QRY_BEGIN
	
	
template <typename T>
struct QryHelper
{
	using THelper = QryHelper<T>;

	inline static thread_local bool IsSuccess;
	inline static thread_local int LastErrorCode;

	static void SetLastErrorCode(int code) {
		LastErrorCode = code;
		IsSuccess = code == 0;
	}

};

	NS_DETAIL_BEGIN
	template <typename T>
	struct IsQryHelper : JCore::FalseType {};

	template <template <typename> typename Base, typename U>
	struct IsQryHelper<Base<U>> : JCore::Conditional_t<JCore::IsSameType_v<Base<U>, QryHelper<U>>, JCore::TrueType, JCore::FalseType> {};
	NS_DETAIL_END

template <typename THelper>
static constexpr bool IsQryHelper_v = Detail::IsQryHelper<THelper>::Value;

NS_QRY_END