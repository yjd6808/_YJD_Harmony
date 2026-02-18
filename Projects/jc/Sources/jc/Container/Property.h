/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 5:30:00 PM
 * =====================
 *
 */

#pragma once

#include <jc/Container/PropertyStatics.h>
#include <jc/Container/PropertyBase.h>
#include <jc/Pool/ObjectPool.h>

NS_JC_BEGIN

template <typename T>
struct Property : PropertyBase, ObjectPool<Property<T>>
{
	static constexpr PropertyType_t TYPE = PropertyTypeGetter<T>::Type;

	static_assert(PropertyType::CAN_CONSTRUCT[TYPE], "... T is not constructable type, so cannot use this property");

	Property()
	: value_(0)
	{
	}

	PropertyType_t GetType() const override
	{
		return TYPE;
	}

	const char* GetTypeName() const override
	{
		return PropertyType::NAME[TYPE];
	}

	int* GetDecayedValue() const override
	{
		return (int*)&value_;
	}

	void Operate(PropertyType_t _argumentType, int* _pDecayedArgument, PropertyBinaryOperatorType_t _operatorType) const override
	{
		PropertyStatics::BinaryOperatorSelectors[TYPE][_argumentType]->Select((int*)&value_, _pDecayedArgument, _operatorType);
	}

	void SetInitialValue() override
	{
		if constexpr (PropertyType::IS_NUMERIC_TYPE[TYPE])
			value_ = 0;
		else if constexpr (TYPE == PropertyType::String)
			value_ = nullptr;    // String::operator=(std::nullptr_t)
		else
			jc_assert(false);  // 올바르지 않은 프로퍼티 타입인 경우 (ex: Unknown, CharPtr 같은)
	}

	T value_;
	static Property<T> Default;
};

NS_END
