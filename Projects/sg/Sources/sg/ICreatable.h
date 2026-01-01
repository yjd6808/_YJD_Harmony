/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 8:43:24 PM
 * =====================
 *
 */


#pragma once

#include <jc/TypeTraits.h>

template <typename T>
struct ICreatable
{
	using TCreatable = ICreatable<T>;

	template <typename... Args>
	static T* CreateInstance(Args&&... _args)
	{
		return dbg_new T(jc::Forward<Args>(_args)...);
	}
};
