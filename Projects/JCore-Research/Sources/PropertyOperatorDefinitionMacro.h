/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 10:27:01 PM
 * =====================
 *
 */


#pragma once

#define SG_OPERATION_DEF_BEGIN(property_type, operation)																	\
	struct PropertyValueBinaryOperator_##property_type##_##operation : IPropertyValueBinaryOperator<property_type>			\
	{																														\
		static constexpr PropertyBinaryOperatorType_t Operation = PropertyBinaryOperatorType::##operation;




#define SG_OPERATION_OPERATE(rhs_type)	void Operate(ValTy& lhs, rhs_type rhs) override
#define SG_OPERATION_BODY(def, ...)		JCORE_EXPAND_1(JCORE_CONCAT_ARGS(SG_OPERATION_BODY_, JCORE_ARGS_COUNT(__VA_ARGS__))(def, __VA_ARGS__))
#define SG_OPERATION_BODY_0(def)		{ def; }
#define SG_OPERATION_BODY_1(def, log)	{ def; log(LeftOperandType, PropertyArgumentDescription<decltype(rhs)>::ArgumentType, Operation); }
#define SG_OPERATION_DEF_END	};