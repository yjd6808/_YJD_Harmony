/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 10:25:34 PM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Container/PropertyOperator.h>
#include <JCore/Container/PropertyOperatorDefinitionMacro.h>
#include <JCore/Container/PropertyStatics.h>

NS_JC_BEGIN

SG_OPERATION_DEF_BEGIN(String, Store)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(String, Move)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(lhs = Move(rhs))
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(lhs = rhs)		// 포인터타입은 리터럴인지 구분할 수 없으므로 복사하도록 함
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(String, Plus)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(String, Minus)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(String, Multiply)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END



SG_OPERATION_DEF_BEGIN(String, Divide)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(String, Modulus)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(String, Equal)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(String, NotEqual)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(String, GreatorEqual)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END


SG_OPERATION_DEF_BEGIN(String, Greator)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(String, LessEqual)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(String, Less)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END


void InitializePropertyOperatorString() {
	PropertyStatics::BinaryOperators_String[PropertyBinaryOperatorType::Store]			= dbg_new PropertyValueBinaryOperator_String_Store{};
	PropertyStatics::BinaryOperators_String[PropertyBinaryOperatorType::Move]			= dbg_new PropertyValueBinaryOperator_String_Move{};
	PropertyStatics::BinaryOperators_String[PropertyBinaryOperatorType::Plus]			= dbg_new PropertyValueBinaryOperator_String_Plus{};
	PropertyStatics::BinaryOperators_String[PropertyBinaryOperatorType::Minus]			= dbg_new PropertyValueBinaryOperator_String_Minus{};
	PropertyStatics::BinaryOperators_String[PropertyBinaryOperatorType::Multiply]		= dbg_new PropertyValueBinaryOperator_String_Multiply{};
	PropertyStatics::BinaryOperators_String[PropertyBinaryOperatorType::Divide]			= dbg_new PropertyValueBinaryOperator_String_Divide{};
	PropertyStatics::BinaryOperators_String[PropertyBinaryOperatorType::Modulus]		= dbg_new PropertyValueBinaryOperator_String_Modulus{};
	PropertyStatics::BinaryOperators_String[PropertyBinaryOperatorType::Equal]			= dbg_new PropertyValueBinaryOperator_String_Equal{};
	PropertyStatics::BinaryOperators_String[PropertyBinaryOperatorType::NotEqual]		= dbg_new PropertyValueBinaryOperator_String_NotEqual{};
	PropertyStatics::BinaryOperators_String[PropertyBinaryOperatorType::GreatorEqual]	= dbg_new PropertyValueBinaryOperator_String_GreatorEqual{};
	PropertyStatics::BinaryOperators_String[PropertyBinaryOperatorType::Greator]		= dbg_new PropertyValueBinaryOperator_String_Greator{};
	PropertyStatics::BinaryOperators_String[PropertyBinaryOperatorType::LessEqual]		= dbg_new PropertyValueBinaryOperator_String_LessEqual{};
	PropertyStatics::BinaryOperators_String[PropertyBinaryOperatorType::Less]			= dbg_new PropertyValueBinaryOperator_String_Less{};

}

NS_JC_END