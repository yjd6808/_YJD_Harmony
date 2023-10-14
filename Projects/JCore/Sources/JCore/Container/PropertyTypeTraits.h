/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 6:06:34 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/PropertyType.h>
#include <JCore/Hasher.h>

NS_JC_BEGIN

// 할당가능한 최적의 타입, 직접 사용 금지, PropertyTypeDescription을 통해서 사용할 것 (const, volatile, 레퍼런스 타입등에 대해서 특수화 안되어있기 때문)
template <PropertyType_t Type>	struct PropertyDataTypeGetter							{ using Ty = void;		};	
template <>						struct PropertyDataTypeGetter<PropertyType::Int64>		{ using Ty = Int64;		};
template <>						struct PropertyDataTypeGetter<PropertyType::Int64U>		{ using Ty = Int64U;	};
template <>						struct PropertyDataTypeGetter<PropertyType::Int>		{ using Ty = Int;		};
template <>						struct PropertyDataTypeGetter<PropertyType::Int32U>		{ using Ty = Int32U;	};
template <>						struct PropertyDataTypeGetter<PropertyType::Int32L>		{ using Ty = Int32L;	};
template <>						struct PropertyDataTypeGetter<PropertyType::Int32UL>	{ using Ty = Int32UL;	};
template <>						struct PropertyDataTypeGetter<PropertyType::Int16>		{ using Ty = Int16;		};
template <>						struct PropertyDataTypeGetter<PropertyType::Int16U>		{ using Ty = Int16U;	};
template <>						struct PropertyDataTypeGetter<PropertyType::Bool>		{ using Ty = bool;		};
template <>						struct PropertyDataTypeGetter<PropertyType::Int8>		{ using Ty = Int8;		};
template <>						struct PropertyDataTypeGetter<PropertyType::Int8U>		{ using Ty = Int8U;		};
template <>						struct PropertyDataTypeGetter<PropertyType::Float>		{ using Ty = Float;		};
template <>						struct PropertyDataTypeGetter<PropertyType::Double>		{ using Ty = Double;	};
template <>						struct PropertyDataTypeGetter<PropertyType::LDouble>	{ using Ty = LDouble;	};
template <>						struct PropertyDataTypeGetter<PropertyType::String>		{ using Ty = String;	};
template <>						struct PropertyDataTypeGetter<PropertyType::CharPtr>	{ using Ty = char*;		};


// 인자로 전달된 타입
template <typename T>	struct PropertyTypeGetter				{ static constexpr PropertyType_t Type = PropertyType::Unknown;	  };
template <>				struct PropertyTypeGetter<Int64>	    { static constexpr PropertyType_t Type = PropertyType::Int64;     };
template <>				struct PropertyTypeGetter<Int64U>	    { static constexpr PropertyType_t Type = PropertyType::Int64U;    };
template <>				struct PropertyTypeGetter<Int>		    { static constexpr PropertyType_t Type = PropertyType::Int;		  };
template <>				struct PropertyTypeGetter<Int32U>	    { static constexpr PropertyType_t Type = PropertyType::Int32U;    };
template <>				struct PropertyTypeGetter<Int32L>	    { static constexpr PropertyType_t Type = PropertyType::Int32L;    };
template <>				struct PropertyTypeGetter<Int32UL>	    { static constexpr PropertyType_t Type = PropertyType::Int32UL;   };
template <>				struct PropertyTypeGetter<Int16>	    { static constexpr PropertyType_t Type = PropertyType::Int16;	  };
template <>				struct PropertyTypeGetter<Int16U>	    { static constexpr PropertyType_t Type = PropertyType::Int16U;    };
template <>				struct PropertyTypeGetter<bool>		    { static constexpr PropertyType_t Type = PropertyType::Bool;	  };
template <>				struct PropertyTypeGetter<Int8>		    { static constexpr PropertyType_t Type = PropertyType::Int8;	  };
template <>				struct PropertyTypeGetter<Int8U>	    { static constexpr PropertyType_t Type = PropertyType::Int8U;	  };
template <>				struct PropertyTypeGetter<Float>	    { static constexpr PropertyType_t Type = PropertyType::Float;	  };
template <>				struct PropertyTypeGetter<Double>	    { static constexpr PropertyType_t Type = PropertyType::Double;    };
template <>				struct PropertyTypeGetter<LDouble>	    { static constexpr PropertyType_t Type = PropertyType::LDouble;   };
template <>				struct PropertyTypeGetter<String>		{ static constexpr PropertyType_t Type = PropertyType::String;	  };
template <>				struct PropertyTypeGetter<char*>		{ static constexpr PropertyType_t Type = PropertyType::CharPtr;	  };

template <Int64U>	struct PropertyBinaryOperatorTypeGetter { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Unknwon; };
template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("+")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Plus;  };
template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("-")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Minus; };
template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("*")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Multiply; };
template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("/")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Divide; };
template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("%")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Modulus; };

template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("+=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Plus; };
template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("-=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Minus; };
template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("*=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Multiply; };
template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("/=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Divide; };
template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("%=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Modulus; };

template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("==")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Equal; };
template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("!=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::NotEqual; };
template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()(">=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::GreatorEqual; };
template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("> ")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Greator; };
template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("<=")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::LessEqual; };
template <>			struct PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()("< ")> { static constexpr PropertyBinaryOperatorType_t Type = PropertyBinaryOperatorType::Less; };

template <typename T>
struct Property;
struct PropertyBase;

template <typename T>
struct PropertyTypeDescription
{
private:
	template <typename Y> struct RemoveConstPtr : RemoveConst<Y> {};
	template <typename Y> struct RemoveConstPtr<Y*> { using Type = Y*; };
	template <typename Y> struct RemoveConstPtr<const Y*> { using Type = Y*; };
public:
	using Ty = Decay_t<typename RemoveConstPtr<NaturalType_t<T>>::Type>;
	static constexpr PropertyType_t Type = PropertyTypeGetter<Ty>::Type;
};


NS_JC_END
