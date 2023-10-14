/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 12:25:05 PM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Container/PropertyFactory.h>
#include <JCore/Container/Property.h>

NS_JC_BEGIN

template <>
struct PropertyFactory<Int> : IPropertyFactory
{
	PropertyBase* CreateInstance() override { return dbg_new Property<Int>; }
};

NS_JC_END