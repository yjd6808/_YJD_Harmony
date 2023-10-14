/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 5:30:00 PM
 * =====================
 *
 */


#pragma once



#include <JCore/Container/PropertyStatics.h>
#include <JCore/Container/PropertyBase.h>
#include <JCore/Pool/ObjectPool.h>

NS_JC_BEGIN

template <typename T>
struct Property : PropertyBase, ObjectPool<Property<T>>
{
	static constexpr PropertyType_t Type = PropertyTypeGetter<T>::Type;

	static_assert(PropertyType::CanConstruct[Type], "... T is not constructable type, so cannot use this property");
	
	Property() : Value(0) {}
	PropertyType_t GetType() const override { return Type; }
	const char* GetTypeName() const override { return PropertyType::Name[Type]; }
	int* GetDecayedValue() const override { return (int*)&Value; }
	void Operate(PropertyType_t argumentType, int* decayedArgument, PropertyBinaryOperatorType_t operatorType) const override {
		PropertyStatics::BinaryOperatorSelectors[Type][argumentType]->Select((int*)&Value, decayedArgument, operatorType);
	}
	void SetInitialValue() override {
		if constexpr (PropertyType::IsNumericType[Type])
			Value = 0;
		else if constexpr (Type == PropertyType::String)
			Value = nullptr;	// String::operator=(std::nullptr_t)
		else
			DebugAssert(false);	// 올바르지 않은 프로퍼티 타입인 경우 (ex: Unknown, CharPtr 같은)
	}

	T Value;
	static Property<T> Default;	
};

NS_JC_END