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


//#define SG_PROPERTY_DEFINITION(property_type)																							\
//template <>																																\
//struct Property<property_type> : PropertyBase, ObjectPool<Property<property_type>>														\
//{																																		\
//	static constexpr PropertyType_t Type = PropertyType::property_type;																	\
//	Property() : Value(0) {}																											\
//	PropertyType_t GetType() const override { return Type; }																			\
//	PropertyArgumentType_t GetArgumentType() const override { return PropertyArgumentDescription<property_type>::ArgumentType; }		\
//	const char* GetTypeName() const override { return PropertyType::Name[Type]; }														\
//	int* GetDecayedValue() const override { return (int*)&Value; }																		\
//	void Operate(PropertyArgumentType_t argumentType, int* decayedArgument, PropertyBinaryOperatorType_t operatorType) override {		\
//		PropertyStatics::BinaryOperatorSelectors[Type][argumentType]->Select((int*)&Value, decayedArgument, operatorType);				\
//	}																																	\
//	property_type Value;																												\
//	static Property<property_type> Default;																								\
//};
//
//SG_PROPERTY_DEFINITION(Int)
//SG_PROPERTY_DEFINITION(Int64)
//SG_PROPERTY_DEFINITION(Float)
//SG_PROPERTY_DEFINITION(Double)
//SG_PROPERTY_DEFINITION(String)
