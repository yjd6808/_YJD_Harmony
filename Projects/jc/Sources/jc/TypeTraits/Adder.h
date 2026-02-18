/*
 * 작성자: 윤정도
 * 생성일: 12/16/2022 12:53:30 PM
 * =====================
 *
 */


#pragma once

#include <jc/Namespace.h>

NS_JC_BEGIN
	NS_DETAIL_BEGIN

	template <typename T>
	struct AddLValueReference
	{
		using Type = T&;
	};

	template <typename T>
	struct AddRValueReference
	{
		using Type = T&&;
	};

	NS_END


template <typename T>
using AddLValueRef_t = typename detail::AddLValueReference<T>::Type;

template <typename T>
using AddRValueRef_t = typename detail::AddRValueReference<T>::Type;


NS_END
