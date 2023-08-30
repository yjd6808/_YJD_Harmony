/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 11:55:55 AM
 * =====================
 *
 */

#include "PropertyStatics.h"
#include "Property.h"

extern void InitializePropertyOperatorSelectors();
extern void InitializePropertyOperatorInt();
extern void InitializePropertyOperatorInt64();
extern void InitializePropertyOperatorFloat();
extern void InitializePropertyOperatorDouble();
extern void InitializePropertyOperatorString();
extern void InitializePropertyOperatorUnary();

void PropertyStatics::Initialize() {
	InitializePropertyOperatorSelectors();
	InitializePropertyOperatorInt();
	InitializePropertyOperatorInt64();
	InitializePropertyOperatorFloat();
	InitializePropertyOperatorDouble();
	InitializePropertyOperatorString();
	InitializePropertyOperatorUnary();

	Factorys[PropertyType::Int]		= dbg_new PropertyFactory<Int>;
	Factorys[PropertyType::Int64]	= dbg_new PropertyFactory<Int64>;
	Factorys[PropertyType::Float]	= dbg_new PropertyFactory<Float>;
	Factorys[PropertyType::Double]	= dbg_new PropertyFactory<Double>;
	Factorys[PropertyType::String]	= dbg_new PropertyFactory<String>;
}


void PropertyStatics::Finalize() {
	// FinalizePropertyOperatorSelectors()
	for (int i = 0; i < PropertyType::Max; ++i) {
		for (int j = 0; j < PropertyArgumentType::Max; ++j) {
			JCORE_DELETE_SAFE(BinaryOperatorSelectors[i][j]);
		}
	}

	// FinalizePropertyOperatorInt()
	// FinalizePropertyOperatorInt64()
	// FinalizePropertyOperatorFloat()
	// FinalizePropertyOperatorDouble()
	// FinalizePropertyOperatorUnary()
	for (int i = 0; i < PropertyBinaryOperatorType::Max; ++i) {
		JCORE_DELETE_SAFE(BinaryOperators_Int[i]);
		JCORE_DELETE_SAFE(BinaryOperators_Int64[i]);
		JCORE_DELETE_SAFE(BinaryOperators_Float[i]);
		JCORE_DELETE_SAFE(BinaryOperators_Double[i]);
		JCORE_DELETE_SAFE(BinaryOperators_String[i]);
	}

	// FinalizePropertyOperatorUnary()
	for (int i = 0; i < PropertyType::Max; ++i) {
		JCORE_DELETE_SAFE(UnaryOperators[i]);
	}

	// FinalizePropertyFactory()
	for (int i = 0; i < PropertyType::Max; ++i) {
		JCORE_DELETE_SAFE(Factorys[i]);
	}

	Property<Int>::FreeAllObjects();
	Property<Int64>::FreeAllObjects();
	Property<Float>::FreeAllObjects();
	Property<Double>::FreeAllObjects();
	Property<String>::FreeAllObjects();
}