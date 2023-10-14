/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 12:20:46 PM
 * =====================
 *
 */


#pragma once

#include "PropertyTypeTraits.h"

NS_JC_BEGIN

struct IPropertyFactory
{
	virtual ~IPropertyFactory() = default;
	virtual PropertyBase* CreateInstance() = 0;
};

#pragma warning(push)
#pragma warning(disable: 4291)

/*
	'void *JCore::ObjectPool<Property<Float>>::operator new(size_t,int,const char *,int)':
    no matching operator delete found; memory will not be freed if initialization throws an exception
*/
template <typename T>
struct PropertyFactory : IPropertyFactory
{
	static_assert(PropertyType::CanConstruct[PropertyTypeGetter<T>::Type], "... T is not constructable property type");
	PropertyBase* CreateInstance() override { return dbg_new Property<T>; }
};


#pragma warning(pop)

NS_JC_END