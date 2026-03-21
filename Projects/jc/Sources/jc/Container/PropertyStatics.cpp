/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 11:55:55 AM
 * =====================
 *
 */


#include "jc/Container/PropertyOperatorDefinitions.h"
#include "jc/Container/PropertyStatics.h"
#include "jc/Container/Property.h"

NS_JC_BEGIN

template <PropertyType_t I>
static void CreateFactory() {
	if constexpr (I == PropertyType::CharPtr) return;
	else {
		PropertyStatics::Factorys[I] = dbg_new PropertyFactory<typename PropertyDataTypeGetter<I>::Ty>();
		CreateFactory<PropertyType_t(I + 1)>();
	}
}

template <PropertyType_t I>
static void InitDefaultProperty() {
	if constexpr (I == PropertyType::CharPtr) return;
	else {
		PropertyStatics::DefaultPrperty[I] = &Property<typename PropertyDataTypeGetter<I>::Ty>::Default;
		InitDefaultProperty<PropertyType_t(I + 1)>();
	}
}


void PropertyStatics::Initialize() {
	InitializePropertyOperatorSelectors();
	InitializePropertyOperatorUnary();

	InitializePropertyOperatorBool();
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
	InitDefaultProperty<PropertyType_t(0)>();
}


void PropertyStatics::Finalize() {
	for (int i = 0; i < PropertyType::Max; ++i) {
		for (int j = 0; j < PropertyType::Max; ++j) {
			JC_DELETE_SAFE(BinaryOperatorSelectors[i][j]);
		}
	}

	for (int i = 0; i < PropertyBinaryOperatorType::Max; ++i) {
		JC_DELETE_SAFE(BinaryOperators_bool[i]);
		JC_DELETE_SAFE(BinaryOperators__s8[i]);
		JC_DELETE_SAFE(BinaryOperators__u8[i]);
		JC_DELETE_SAFE(BinaryOperators__s16[i]);
		JC_DELETE_SAFE(BinaryOperators__u16[i]);
		JC_DELETE_SAFE(BinaryOperators__s32[i]);
		JC_DELETE_SAFE(BinaryOperators__u32[i]);
		JC_DELETE_SAFE(BinaryOperators__s32l[i]);
		JC_DELETE_SAFE(BinaryOperators__u32l[i]);
		JC_DELETE_SAFE(BinaryOperators__s64[i]);
		JC_DELETE_SAFE(BinaryOperators__u64[i]);
		JC_DELETE_SAFE(BinaryOperators__f32[i]);
		JC_DELETE_SAFE(BinaryOperators__f64[i]);
		JC_DELETE_SAFE(BinaryOperators__f64l[i]);
		JC_DELETE_SAFE(BinaryOperators_String[i]);
	}

	for (int i = 0; i < PropertyType::Max; ++i) {
		JC_DELETE_SAFE(UnaryOperators[i]);
	}

	for (int i = 0; i < PropertyType::Max; ++i) {
		JC_DELETE_SAFE(Factorys[i]);
	}

	Property<bool>::FreeAllObjects();
	Property<_s8>::FreeAllObjects();
	Property<_u8>::FreeAllObjects();
	Property<_s16>::FreeAllObjects();
	Property<_u16>::FreeAllObjects();
	Property<_s32>::FreeAllObjects();
	Property<_u32>::FreeAllObjects();
	Property<_s32l>::FreeAllObjects();
	Property<_u32l>::FreeAllObjects();
	Property<_s64>::FreeAllObjects();
	Property<_u64>::FreeAllObjects();
	Property<_f32>::FreeAllObjects();
	Property<_f64>::FreeAllObjects();
	Property<_f64l>::FreeAllObjects();
	Property<String>::FreeAllObjects();
}

NS_END
