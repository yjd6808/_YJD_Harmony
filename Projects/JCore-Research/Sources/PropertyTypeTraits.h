/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 6:06:34 PM
 * =====================
 *
 */


#pragma once

#include "PropertyType.h"

template <typename T>	struct PropertyTypeGetter				{ using Ty = void   ;	static constexpr PropertyType_t Type = PropertyType::Unknown;	};
template <>				struct PropertyTypeGetter<Int>		    { using Ty = Int	;	static constexpr PropertyType_t Type = PropertyType::Int;		};
template <>				struct PropertyTypeGetter<Int64>	    { using Ty = Int64	;	static constexpr PropertyType_t Type = PropertyType::Int64;		};
template <>				struct PropertyTypeGetter<Float>	    { using Ty = Float	;	static constexpr PropertyType_t Type = PropertyType::Float;		};
template <>				struct PropertyTypeGetter<Double>	    { using Ty = Double	;	static constexpr PropertyType_t Type = PropertyType::Double;	};
template <>				struct PropertyTypeGetter<String>		{ using Ty = String	;	static constexpr PropertyType_t Type = PropertyType::String;	};


// 할당가능한 최적의 타입, 직접 사용 금지, PropertyArgumentDescription을 통해서 사용할 것 (const, volatile, 레퍼런스 타입등에 대해서 특수화 안되어있기 때문)
template <typename T>	struct PropertyTypeOptimal				{ using Ty = void   ;	static constexpr PropertyType_t Type = PropertyType::Unknown;	};
template <>				struct PropertyTypeOptimal<Int64>	    { using Ty = Int64	;	static constexpr PropertyType_t Type = PropertyType::Int64;		};
template <>				struct PropertyTypeOptimal<Int64U>	    { using Ty = Int64	;	static constexpr PropertyType_t Type = PropertyType::Int64;		};
template <>				struct PropertyTypeOptimal<Int>		    { using Ty = Int	;	static constexpr PropertyType_t Type = PropertyType::Int;		};
template <>				struct PropertyTypeOptimal<Int32U>	    { using Ty = Int	;	static constexpr PropertyType_t Type = PropertyType::Int;		};
template <>				struct PropertyTypeOptimal<Int32L>	    { using Ty = Int	;	static constexpr PropertyType_t Type = PropertyType::Int;		};
template <>				struct PropertyTypeOptimal<Int32UL>	    { using Ty = Int	;	static constexpr PropertyType_t Type = PropertyType::Int;		};
template <>				struct PropertyTypeOptimal<Int16>	    { using Ty = Int	;	static constexpr PropertyType_t Type = PropertyType::Int;		};
template <>				struct PropertyTypeOptimal<Int16U>	    { using Ty = Int	;	static constexpr PropertyType_t Type = PropertyType::Int;		};
template <>				struct PropertyTypeOptimal<Int8>		{ using Ty = Int	;	static constexpr PropertyType_t Type = PropertyType::Int;		};
template <>				struct PropertyTypeOptimal<Int8U>	    { using Ty = Int	;	static constexpr PropertyType_t Type = PropertyType::Int;		};
template <>				struct PropertyTypeOptimal<WideChar>    { using Ty = Int	;	static constexpr PropertyType_t Type = PropertyType::Int;		};
template <>				struct PropertyTypeOptimal<Float>	    { using Ty = Float	;	static constexpr PropertyType_t Type = PropertyType::Float;		};
template <>				struct PropertyTypeOptimal<Double>	    { using Ty = Double	;	static constexpr PropertyType_t Type = PropertyType::Double;	};
template <>				struct PropertyTypeOptimal<LDouble>	    { using Ty = Double	;	static constexpr PropertyType_t Type = PropertyType::Double;	};
template <>				struct PropertyTypeOptimal<char*>		{ using Ty = String	;	static constexpr PropertyType_t Type = PropertyType::String;	};
template <>				struct PropertyTypeOptimal<String>		{ using Ty = String	;	static constexpr PropertyType_t Type = PropertyType::String;	};


template <PropertyType_t Type>	struct PropertyDataTypeGetter						{ using Ty = void;	};	
template <>						struct PropertyDataTypeGetter<PropertyType::Int>	{ using Ty = Int; 	};
template <>						struct PropertyDataTypeGetter<PropertyType::Int64>	{ using Ty = Int64; };
template <>						struct PropertyDataTypeGetter<PropertyType::Float>	{ using Ty = Float; };
template <>						struct PropertyDataTypeGetter<PropertyType::Double>	{ using Ty = Double;}; 
template <>						struct PropertyDataTypeGetter<PropertyType::String>	{ using Ty = String;};	



// 인자로 전달된 타입
template <typename T>	struct PropertyArgumentTypeGetter				{ static constexpr PropertyArgumentType_t Type = PropertyArgumentType::Unknown;	};
template <>				struct PropertyArgumentTypeGetter<Int64>	    { static constexpr PropertyArgumentType_t Type = PropertyArgumentType::Int64;     };
template <>				struct PropertyArgumentTypeGetter<Int64U>	    { static constexpr PropertyArgumentType_t Type = PropertyArgumentType::Int64U;    };
template <>				struct PropertyArgumentTypeGetter<Int>		    { static constexpr PropertyArgumentType_t Type = PropertyArgumentType::Int;	    };
template <>				struct PropertyArgumentTypeGetter<Int32U>	    { static constexpr PropertyArgumentType_t Type = PropertyArgumentType::Int32U;    };
template <>				struct PropertyArgumentTypeGetter<Int32L>	    { static constexpr PropertyArgumentType_t Type = PropertyArgumentType::Int32L;    };
template <>				struct PropertyArgumentTypeGetter<Int32UL>	    { static constexpr PropertyArgumentType_t Type = PropertyArgumentType::Int32UL;   };
template <>				struct PropertyArgumentTypeGetter<Int16>	    { static constexpr PropertyArgumentType_t Type = PropertyArgumentType::Int16;	    };
template <>				struct PropertyArgumentTypeGetter<Int16U>	    { static constexpr PropertyArgumentType_t Type = PropertyArgumentType::Int16U;    };
template <>				struct PropertyArgumentTypeGetter<Int8>		    { static constexpr PropertyArgumentType_t Type = PropertyArgumentType::Int8;	    };
template <>				struct PropertyArgumentTypeGetter<Int8U>	    { static constexpr PropertyArgumentType_t Type = PropertyArgumentType::Int8U;	    };
template <>				struct PropertyArgumentTypeGetter<Float>	    { static constexpr PropertyArgumentType_t Type = PropertyArgumentType::Float;	    };
template <>				struct PropertyArgumentTypeGetter<Double>	    { static constexpr PropertyArgumentType_t Type = PropertyArgumentType::Double;    };
template <>				struct PropertyArgumentTypeGetter<LDouble>	    { static constexpr PropertyArgumentType_t Type = PropertyArgumentType::LDouble;   };
template <>				struct PropertyArgumentTypeGetter<String>		{ static constexpr PropertyArgumentType_t Type = PropertyArgumentType::String;	};
template <>				struct PropertyArgumentTypeGetter<char*>		{ static constexpr PropertyArgumentType_t Type = PropertyArgumentType::CharPtr;	};


template <PropertyArgumentType_t Type>	struct PropertyArgumentDataTypeGetter											{ using Ty = void;		};
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::Int64>				{ using Ty = Int64;		};
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::Int64U>				{ using Ty = Int64U;    }; 
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::Int>				{ using Ty = Int;     	};
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::Int32U>				{ using Ty = Int32U;    }; 
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::Int32L>				{ using Ty = Int32L;    }; 
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::Int32UL>			{ using Ty = Int32UL;   };  
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::Int16>				{ using Ty = Int16;     };
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::Int16U>				{ using Ty = Int16U;    }; 
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::Int8>				{ using Ty = Int8;     	};
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::Int8U>				{ using Ty = Int8U;     };
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::Float>				{ using Ty = Float;     };
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::Double>				{ using Ty = Double;    }; 
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::LDouble>			{ using Ty = LDouble;   };  
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::String>				{ using Ty = String; 	};
template <>								struct PropertyArgumentDataTypeGetter<PropertyArgumentType::CharPtr>			{ using Ty = char*; 	};


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
struct PropertyArgumentDescription
{
private:
	template <typename Y> struct RemoveConstPtr : RemoveConst<Y> {};
	template <typename Y> struct RemoveConstPtr<Y*> { using Type = Y*; };
	template <typename Y> struct RemoveConstPtr<const Y*> { using Type = Y*; };

	
public:
	using Ty = Decay_t<typename RemoveConstPtr<NaturalType_t<T>>::Type>;
	using OptimalTy = typename PropertyTypeOptimal<Ty>::Ty;
	

	static constexpr PropertyType_t Type = PropertyTypeOptimal<Ty>::Type;
	static constexpr PropertyArgumentType_t ArgumentType = PropertyArgumentTypeGetter<Ty>::Type;
};

