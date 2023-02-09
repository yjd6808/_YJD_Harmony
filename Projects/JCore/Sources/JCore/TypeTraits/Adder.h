/*
 * 작성자: 윤정도
 * 생성일: 12/16/2022 12:53:30 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Namespace.h>

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

	NS_DETAIL_END


template <typename T>
using AddLValueRef_t = typename Detail::AddLValueReference<T>::Type;

template <typename T>
using AddRValueRef_t = typename Detail::AddRValueReference<T>::Type;


NS_JC_END