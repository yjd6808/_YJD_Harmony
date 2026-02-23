/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 6:06:34 PM
 * =====================
 *
 */


#pragma once

#include <jc/Container/PropertyType.h>
#include <jc/Hasher.h>

NS_JC_BEGIN

// 할당가능한 최적의 타입, 직접 사용 금지, PropertyTypeDescription을 통해서 사용할 것 (const, volatile, 레퍼런스 타입등에 대해서 특수화 안되어있기 때문)
template <PropertyType_t Type> struct PropertyDataTypeGetter { using Ty = void; };
template <> struct PropertyDataTypeGetter<PropertyType::_s64> { using Ty = _s64; };
template <> struct PropertyDataTypeGetter<PropertyType::_u64> { using Ty = _u64; };
template <> struct PropertyDataTypeGetter<PropertyType::_s32> { using Ty = _s32; };
template <> struct PropertyDataTypeGetter<PropertyType::_u32> { using Ty = _u32; };
template <> struct PropertyDataTypeGetter<PropertyType::_s32l> { using Ty = _s32l; };
template <> struct PropertyDataTypeGetter<PropertyType::_u32l> { using Ty = _u32l; };
template <> struct PropertyDataTypeGetter<PropertyType::_s16> { using Ty = _s16; };
template <> struct PropertyDataTypeGetter<PropertyType::_u16> { using Ty = _u16; };
template <> struct PropertyDataTypeGetter<PropertyType::Bool> { using Ty = bool; };
template <> struct PropertyDataTypeGetter<PropertyType::_s8> { using Ty = _s8; };
template <> struct PropertyDataTypeGetter<PropertyType::_u8> { using Ty = _u8; };
template <> struct PropertyDataTypeGetter<PropertyType::_f32> { using Ty = _f32; };
template <> struct PropertyDataTypeGetter<PropertyType::_f64> { using Ty = _f64; };
template <> struct PropertyDataTypeGetter<PropertyType::_f64l> { using Ty = _f64l; };
template <> struct PropertyDataTypeGetter<PropertyType::String> { using Ty = String; };
template <> struct PropertyDataTypeGetter<PropertyType::CharPtr> { using Ty = char*; };


// 인자로 전달된 타입
template <typename T> struct PropertyTypeGetter { static constexpr PropertyType_t Type = PropertyType::Unknown; };
template <> struct PropertyTypeGetter<_s64> { static constexpr PropertyType_t Type = PropertyType::_s64; };
template <> struct PropertyTypeGetter<_u64> { static constexpr PropertyType_t Type = PropertyType::_u64; };
template <> struct PropertyTypeGetter<_s32> { static constexpr PropertyType_t Type = PropertyType::_s32; };
template <> struct PropertyTypeGetter<_u32> { static constexpr PropertyType_t Type = PropertyType::_u32; };
template <> struct PropertyTypeGetter<_s32l> { static constexpr PropertyType_t Type = PropertyType::_s32l; };
template <> struct PropertyTypeGetter<_u32l> { static constexpr PropertyType_t Type = PropertyType::_u32l; };
template <> struct PropertyTypeGetter<_s16> { static constexpr PropertyType_t Type = PropertyType::_s16; };
template <> struct PropertyTypeGetter<_u16> { static constexpr PropertyType_t Type = PropertyType::_u16; };
template <> struct PropertyTypeGetter<bool> { static constexpr PropertyType_t Type = PropertyType::Bool; };
template <> struct PropertyTypeGetter<_s8> { static constexpr PropertyType_t Type = PropertyType::_s8; };
template <> struct PropertyTypeGetter<_u8> { static constexpr PropertyType_t Type = PropertyType::_u8; };
template <> struct PropertyTypeGetter<_f32> { static constexpr PropertyType_t Type = PropertyType::_f32; };
template <> struct PropertyTypeGetter<_f64> { static constexpr PropertyType_t Type = PropertyType::_f64; };
template <> struct PropertyTypeGetter<_f64l> { static constexpr PropertyType_t Type = PropertyType::_f64l; };
template <> struct PropertyTypeGetter<String> { static constexpr PropertyType_t Type = PropertyType::String; };
template <> struct PropertyTypeGetter<char*> { static constexpr PropertyType_t Type = PropertyType::CharPtr; };

template <_u64> struct PropertyBinaryOperatorTypeGetter { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Unknwon; };
template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("+")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Plus; };
template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("-")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Minus; };
template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("*")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Multiply; };
template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("/")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Divide; };
template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("%")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Modulus; };

template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("+=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Plus; };
template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("-=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Minus; };
template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("*=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Multiply; };
template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("/=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Divide; };
template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("%=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Modulus; };

template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("==")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Equal; };
template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("!=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::NotEqual; };
template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()(">=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::GreatorEqual; };
template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("> ")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Greator; };
template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("<=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::LessEqual; };
template <> struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("< ")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Less; };

template <typename T>
struct Property;
struct PropertyBase;

template <typename T>
struct PropertyTypeDescription
{
private:
	template <typename Y> struct RemoveConstPtr : RemoveConst<Y> {};
	template <typename Y> struct RemoveConstPtr<Y*>
	{
		using Type = Y*;
	};
	template <typename Y> struct RemoveConstPtr<const Y*>
	{
		using Type = Y*;
	};
public:
	using Ty = Decay_t<typename RemoveConstPtr<NaturalType_t<T>>::Type>;
	static constexpr PropertyType_t Type = PropertyTypeGetter<Ty>::Type;
};


NS_END
