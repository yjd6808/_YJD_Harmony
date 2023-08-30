/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 5:30:00 PM
 * =====================
 *
 */


#pragma once



#include "PropertyStatics.h"
#include "PropertyBase.h"

template <typename T>
struct Property : PropertyBase, ObjectPool<Property<T>>
{
	static constexpr PropertyType_t Type = PropertyTypeGetter<T>::Type;
	static_assert(Type != PropertyType::Unknown, "... T is not property type");
	
	Property() : Value(0) {}
	PropertyType_t GetType() const override { return Type; }
	PropertyArgumentType_t GetArgumentType() const override { return PropertyArgumentDescription<T>::ArgumentType; }
	const char* GetTypeName() const override { return PropertyType::Name[Type]; }
	int* GetDecayedValue() const override { return (int*)&Value; }
	void Operate(PropertyArgumentType_t argumentType, int* decayedArgument, PropertyBinaryOperatorType_t operatorType) override {
		PropertyStatics::BinaryOperatorSelectors[Type][argumentType]->Select((int*)&Value, decayedArgument, operatorType);
	}

	T Value;
	static Property<T> Default;	
};