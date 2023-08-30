/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 12:20:46 PM
 * =====================
 *
 */


#pragma once

#include "PropertyTypeTraits.h"

struct IPropertyFactory
{
	virtual ~IPropertyFactory() = default;
	virtual PropertyBase* CreateInstance() = 0;
};

template <typename T>
struct PropertyFactory : IPropertyFactory
{
	static_assert(PropertyTypeGetter<T>::Type != PropertyType::Unknown, "... T is not property type");
	PropertyBase* CreateInstance() override { return dbg_new Property<T>; }
};

