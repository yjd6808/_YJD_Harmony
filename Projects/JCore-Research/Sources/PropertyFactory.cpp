/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 12:25:05 PM
 * =====================
 *
 */


#include "PropertyFactory.h"
#include "Property.h"

template <>
struct PropertyFactory<Int> : IPropertyFactory
{
	PropertyBase* CreateInstance() override { return dbg_new Property<Int>; }
};
