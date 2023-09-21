/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 11:55:55 AM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Container/PropertyStatics.h>
#include <JCore/Container/Property.h>

NS_JC_BEGIN

extern void InitializePropertyOperatorSelectors();
extern void InitializePropertyOperatorInt8();
extern void InitializePropertyOperatorInt8U();
extern void InitializePropertyOperatorInt16();
extern void InitializePropertyOperatorInt16U();
extern void InitializePropertyOperatorInt();
extern void InitializePropertyOperatorInt32U();
extern void InitializePropertyOperatorInt32L();
extern void InitializePropertyOperatorInt32UL();
extern void InitializePropertyOperatorInt64();
extern void InitializePropertyOperatorInt64U();
extern void InitializePropertyOperatorFloat();
extern void InitializePropertyOperatorDouble();
extern void InitializePropertyOperatorLDouble();
extern void InitializePropertyOperatorString();
extern void InitializePropertyOperatorUnary();

template <PropertyType_t I>
static void CreateFactory() {
	if constexpr (I == PropertyType::CharPtr) return;
	else {
		PropertyStatics::Factorys[I] = dbg_new PropertyFactory<typename PropertyDataTypeGetter<I>::Ty>();
		CreateFactory<PropertyType_t(I + 1)>();
	}
	
}


void PropertyStatics::Initialize() {
	InitializePropertyOperatorSelectors();
	InitializePropertyOperatorUnary();

	InitializePropertyOperatorInt8();
	InitializePropertyOperatorInt8U();
	InitializePropertyOperatorInt16();
	InitializePropertyOperatorInt16U();
	InitializePropertyOperatorInt();
	InitializePropertyOperatorInt32U();
	InitializePropertyOperatorInt32L();
	InitializePropertyOperatorInt32UL();
	InitializePropertyOperatorInt64();
	InitializePropertyOperatorInt64U();
	InitializePropertyOperatorFloat();
	InitializePropertyOperatorDouble();
	InitializePropertyOperatorLDouble();
	InitializePropertyOperatorString();

	CreateFactory<PropertyType_t(0)>();
}


void PropertyStatics::Finalize() {
	for (int i = 0; i < PropertyType::Max; ++i) {
		for (int j = 0; j < PropertyType::Max; ++j) {
			JCORE_DELETE_SAFE(BinaryOperatorSelectors[i][j]);
		}
	}

	for (int i = 0; i < PropertyBinaryOperatorType::Max; ++i) {
		JCORE_DELETE_SAFE(BinaryOperators_Int8[i]);
		JCORE_DELETE_SAFE(BinaryOperators_Int8U[i]);
		JCORE_DELETE_SAFE(BinaryOperators_Int16[i]);
		JCORE_DELETE_SAFE(BinaryOperators_Int16U[i]);
		JCORE_DELETE_SAFE(BinaryOperators_Int[i]);
		JCORE_DELETE_SAFE(BinaryOperators_Int32U[i]);
		JCORE_DELETE_SAFE(BinaryOperators_Int32L[i]);
		JCORE_DELETE_SAFE(BinaryOperators_Int32UL[i]);
		JCORE_DELETE_SAFE(BinaryOperators_Int64[i]);
		JCORE_DELETE_SAFE(BinaryOperators_Int64U[i]);
		JCORE_DELETE_SAFE(BinaryOperators_Float[i]);
		JCORE_DELETE_SAFE(BinaryOperators_Double[i]);
		JCORE_DELETE_SAFE(BinaryOperators_LDouble[i]);
		JCORE_DELETE_SAFE(BinaryOperators_String[i]);
	}

	for (int i = 0; i < PropertyType::Max; ++i) {
		JCORE_DELETE_SAFE(UnaryOperators[i]);
	}

	for (int i = 0; i < PropertyType::Max; ++i) {
		JCORE_DELETE_SAFE(Factorys[i]);
	}

	Property<Int>::FreeAllObjects();
	Property<Int64>::FreeAllObjects();
	Property<Float>::FreeAllObjects();
	Property<Double>::FreeAllObjects();
	Property<String>::FreeAllObjects();
}

NS_JC_END