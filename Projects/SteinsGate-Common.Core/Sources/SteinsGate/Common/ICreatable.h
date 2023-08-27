/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 8:43:24 PM
 * =====================
 *
 */


#pragma once

#include <JCore/TypeTraits.h>

template <typename T>
struct ICreatable
{
	using TCreatable = ICreatable<T>;
	template <typename... Args>
	static T* CreateInstance(Args&&... args) {
		return dbg_new T( JCore::Forward<Args>(args)... );
	}
};