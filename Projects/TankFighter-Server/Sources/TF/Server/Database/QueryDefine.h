/*
 * 작성자: 윤정도
 * 생성일: 5/20/2023 9:54:20 AM
 * =====================
 */

#pragma once

#include <JCore/TypeTraits.h>
#include <JCore/Macro.h>
#include <JCore/Logger/LoggerDefine.h>

#include <TF/Server/Namespace.h>
#include <TF/Server/Const.h>

#include <TF/Server/Database/MysqlDatabase.h>
#include <TF/Server/Database/StatementType.h>
#include <TF/Server/Database/QueryHelper.h>

NS_QRY_BEGIN

template <typename TQryResult>
struct IsVisitable : JCore::FalseType {};

template <typename TQryResult>
struct Visitable
{
	static_assert(IsVisitable<TQryResult>::value, "... T is not visitable Qry Struct");
	constexpr static void BindSelectResult(JCORE_IN TQryResult& v, MysqlQuerySelect* query) {}
	constexpr static int FieldCount = 0;
};


template <typename TQryResult>
constexpr int GetSelectResultFieldCount() {
	return Visitable<TQryResult>::FieldCount;
}

template <typename TQryResult>
constexpr int GetSelectResultFieldCount(const TQryResult& v) {
	return Visitable<TQryResult>::FieldCount;
}

template <typename TQryResultT>
constexpr void BindSelectResult(TQryResultT&& v, MysqlQuerySelect* query) {
	Visitable<JCore::NakedType_t<TQryResultT>>::BindSelectResult(JCore::Forward<TQryResultT>(v), query);
}


#define QRY_STRUCT_FIELD_NAMES(...)		JCORE_EVEN_ARGS(__VA_ARGS__)
#define QRY_STRUCT_VARIABLE_NAMES(...)	JCORE_ODD_ARGS(__VA_ARGS__)

// 쿼리 구조체 적용자
#define QRY_STRUCT_APPLY_FN_DEF(pair)						QRY_STRUCT_APPLY_FN_DEF_(JCORE_EXPAND_ARGS pair)		// (1,2) => 1,2로 벗겨서 전달
#define QRY_STRUCT_APPLY_FN_DEF_(...)						JCORE_EXPAND_1(QRY_STRUCT_APPLY_FN_DEF__(__VA_ARGS__))	// MSVC는 __VA_ARGS__가 하나의 인자로 취급되므로 확장시켜서 전달
#define QRY_STRUCT_APPLY_FN_DEF__(variableName, fieldName)	SelectResultBinder::BindField(fieldName, v.variableName, query);
#define QRY_STRUCT_APPLY(...) JCORE_APPLY_FN(QRY_STRUCT_APPLY_FN_DEF, JCORE_GROUP_OF(2, __VA_ARGS__))

// 결과 바인딩이 가능하도록 쿼리 구조체 등록 (무조건 Qry 네임스페이스 내부에서 사용할 것)
#define REGISTER_QRY_STRUCT(struct_name, ...)																			\
																														\
template <>																												\
struct IsVisitable<struct_name> : JCore::TrueType {};																	\
																														\
template<>																												\
struct Visitable<struct_name>																							\
{																														\
	enum Fields { QRY_STRUCT_VARIABLE_NAMES(__VA_ARGS__), _Count };														\
																														\
																														\
	constexpr static void BindSelectResult(JCORE_IN struct_name& v,  MysqlQuerySelect* query) {							\
		QRY_STRUCT_APPLY(__VA_ARGS__)																					\
	}																													\
																														\
	constexpr static int FieldCount = Fields::_Count;																	\
};																														\

#define QRY_RESULT_DEBUG_ASSERT DebugAssertMsg(Query != nullptr, "쿼리 변수가 NULL임");

struct Result
{
	int LastErrorCode = Const::InvalidValue;
	int RowCount{};
	bool Success{};
};

template <typename TQryResult>
struct SelectResult : Result
{
	MysqlQuerySelectPtr Query;
	bool HasBindedResult = false;
	

	bool HasNext() const {
		QRY_RESULT_DEBUG_ASSERT
		if (!Success) return false;
		return Query->HasNext();
	}

	void FetchCurrentRow() {
		QRY_RESULT_DEBUG_ASSERT
		TQryResult& result = static_cast<TQryResult&>(*this);
		BindSelectResult(result, Query.GetPtr());
		HasBindedResult = true;
	}

	bool FetchNextRow() {
		QRY_RESULT_DEBUG_ASSERT
		if (!Success) return false;

		if (Query->Next()) {
			TQryResult& result = static_cast<TQryResult&>(*this);
			BindSelectResult(result, Query.GetPtr());
			HasBindedResult = true;
			return true;
		}

		HasBindedResult = false;
		return false;
	}
};

struct SelectResultBinder
{
	template <typename TField>
	constexpr static void BindField(const char* fieldName, TField& fieldVariable, MysqlQuerySelect* executedQuery) {
		if constexpr (JCore::IsString_v<TField>) {
			fieldVariable = executedQuery->GetRawString(fieldName);
		} else if constexpr (
			JCore::Or_v<
				JCore::IsIntegerType_v<TField>, 
				JCore::IsCharaterType_v<TField>, 
				JCore::IsFloatType_v<TField>
			>) {
			fieldVariable = executedQuery->GetNumber<TField>(fieldName);
		} else if constexpr (JCore::IsDateTime_v<TField>) {
			fieldVariable = executedQuery->GetDateTime(fieldName);
		} else {
			DebugAssertMsg(false, "바인딩 할 수 없는 필드입니다. (TField = %s)", typeid(TField).name());
		}
	}
};



template <typename TQry>
struct SelectStatement
{
	template <typename THelper, typename TResult, typename... Args>
	constexpr static void Execute(MysqlDatabase* database, TResult& result, Args&&... args) {
		static_assert(IsQryHelper_v<THelper>, "... THelper is not QueryHelper<T>");

		THelper::SetLastErrorCode(Const::InvalidValue);
		static_assert(JCore::StringUtil::CTCountChar(TQry::Script, '?') == sizeof...(args));
		/*if constexpr (JCore::StringUtil::CTCountChar(TQry::Script, '?') != sizeof...(args)) {
			_LogWarn_("쿼리 스크립트에서 요구하는 인자갯수와 전달받은 인자 갯수가 틀립니다.");
			return;
		}*/

		auto spQuery = database->Query(TQry::Script, JCore::Forward<Args>(args)...);

		if (spQuery == nullptr) {
			return;
		}

		DebugAssertMsg(spQuery->GetStatementType() == StatementType::Select, "셀렉트 스테이트먼트가 아닙니다.");

		const int iErrorCode = spQuery->GetErrorCode();

		result.Query = spQuery;
		result.Success = iErrorCode == 0;
		result.LastErrorCode = iErrorCode;
		result.RowCount = iErrorCode == 0 ? spQuery->GetRowCount() : 0;

		THelper::SetLastErrorCode(iErrorCode);

		if (!result.Query->HasNext()) {
			return;
		}

		result.FetchCurrentRow();
	}
};

struct InsertResult : Result
{
	MysqlQueryInsertPtr Query;

	Int64U GetInsertId() {
		QRY_RESULT_DEBUG_ASSERT
		if (Query->IsFailed()) return 0;
		return Query->GetInsertId();
	}
};

struct DeleteResult : Result
{
	MysqlQueryDeletePtr Query;
};

struct UpdateResult : Result
{
	MysqlQueryUpdatePtr Query;
};


template <StatementType StatementType> struct ResultTyGetter { using Ty = void;		    };
template <> struct ResultTyGetter<StatementType::Insert>	 { using Ty = InsertResult; };
template <> struct ResultTyGetter<StatementType::Delete>	 { using Ty = DeleteResult; };
template <> struct ResultTyGetter<StatementType::Update>	 { using Ty = UpdateResult; };


template <StatementType StatementType, typename TQry>
struct Statement
{
	template <typename THelper, typename... Args>
	constexpr static void Execute(MysqlDatabase* database, typename ResultTyGetter<StatementType>::Ty& result, Args&&... args) {
		static_assert(IsQryHelper_v<THelper>, "... THelper is not QueryHelper<T>");

		THelper::SetLastErrorCode(Const::InvalidValue);
		static_assert(JCore::StringUtil::CTCountChar(TQry::Script, '?') == sizeof...(args));
		/*if constexpr (JCore::StringUtil::CTCountChar(TQry::Script, '?') != sizeof...(args)) {
			_LogWarn_("쿼리 스크립트에서 요구하는 인자갯수와 전달받은 인자 갯수가 틀립니다.");
			return;
		}*/

		auto spQuery = database->Query(TQry::Script, JCore::Forward<Args>(args)...);

		if (spQuery == nullptr) {
			return;
		}

		DebugAssertMsg(spQuery->GetStatementType() == StatementType, "%s 스테이트먼트가 아닙니다.", StatementName(StatementType));
		const int iErrorCode = spQuery->GetErrorCode();

		result.Query = spQuery;
		result.Success = iErrorCode == 0;
		result.LastErrorCode = iErrorCode;
		result.RowCount = iErrorCode == 0 ? spQuery->GetRowCount() : 0;

		THelper::SetLastErrorCode(spQuery->GetErrorCode());
	}
};



NS_QRY_END

#define QRY_SELECT_STATEMENT_BEGIN(struct_name) struct struct_name : SelectStatement<struct_name> {
#define QRY_SELECT_STATEMENT_END };

#define QRY_SELECT_RESULT_BEGIN(struct_name) struct struct_name : SelectResult<struct_name> {
#define QRY_SELECT_RESULT_END };


#define QRY_INSERT_STATEMENT_BEGIN(struct_name) struct struct_name : Statement<StatementType::Insert, struct_name> {
#define QRY_INSERT_STATEMENT_END };

#define QRY_DELETE_STATEMENT_BEGIN(struct_name) struct struct_name : Statement<StatementType::Delete, struct_name> {
#define QRY_DELETE_STATEMENT_END };

#define QRY_UPDATE_STATEMENT_BEGIN(struct_name) struct struct_name : Statement<StatementType::Update, struct_name> {
#define QRY_UPDATE_STATEMENT_END };


// 스크립트 작성시 중간에 공백, 라인브레이크 등 신경쓰기 번거롭고 따옴표 달기도 번거로워서 스크립트로 매크로를 만듬. 2023/10/17
// 1. 매크로는 줄바꿈 + 줄바꿈 이후의 하나의 공백으로 처리한다.
// 2. 매크로는 콤마를 인자로 전달하면 콤마가 사라진다. 따라서 사라진 콤마를 다시 재생성해줘야한다.

#define QRY_SCRIPT(...)				JCORE_EXPAND_1(JCORE_CONCAT_3(QRY_SCRIPT_, JCORE_ARGS_COUNT(__VA_ARGS__), (__VA_ARGS__)))
#define QRY_SCRIPT_1(_1) \
    #_1;
#define QRY_SCRIPT_2(_1, _2) \
    #_1 ", " #_2;
#define QRY_SCRIPT_3(_1, _2, _3) \
    #_1 ", " #_2 ", " #_3;
#define QRY_SCRIPT_4(_1, _2, _3, _4) \
    #_1 ", " #_2 ", " #_3 ", " #_4;
#define QRY_SCRIPT_5(_1, _2, _3, _4, _5) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5;
#define QRY_SCRIPT_6(_1, _2, _3, _4, _5, _6) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6;
#define QRY_SCRIPT_7(_1, _2, _3, _4, _5, _6, _7) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7;
#define QRY_SCRIPT_8(_1, _2, _3, _4, _5, _6, _7, _8) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8;
#define QRY_SCRIPT_9(_1, _2, _3, _4, _5, _6, _7, _8, _9) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9;
#define QRY_SCRIPT_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10;
#define QRY_SCRIPT_11(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11;
#define QRY_SCRIPT_12(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12;
#define QRY_SCRIPT_13(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13;
#define QRY_SCRIPT_14(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14;
#define QRY_SCRIPT_15(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15;
#define QRY_SCRIPT_16(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16;
#define QRY_SCRIPT_17(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16 ", " #_17;
#define QRY_SCRIPT_18(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16 ", " #_17 ", " #_18;
#define QRY_SCRIPT_19(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16 ", " #_17 ", " #_18 ", " #_19;
#define QRY_SCRIPT_20(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16 ", " #_17 ", " #_18 ", " #_19 ", " #_20;
#define QRY_SCRIPT_21(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16 ", " #_17 ", " #_18 ", " #_19 ", " #_20 ", " #_21;
#define QRY_SCRIPT_22(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16 ", " #_17 ", " #_18 ", " #_19 ", " #_20 ", " #_21 ", " #_22;
#define QRY_SCRIPT_23(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16 ", " #_17 ", " #_18 ", " #_19 ", " #_20 ", " #_21 ", " #_22 ", " #_23;
#define QRY_SCRIPT_24(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16 ", " #_17 ", " #_18 ", " #_19 ", " #_20 ", " #_21 ", " #_22 ", " #_23 ", " #_24;
#define QRY_SCRIPT_25(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16 ", " #_17 ", " #_18 ", " #_19 ", " #_20 ", " #_21 ", " #_22 ", " #_23 ", " #_24 ", " #_25;
#define QRY_SCRIPT_26(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16 ", " #_17 ", " #_18 ", " #_19 ", " #_20 ", " #_21 ", " #_22 ", " #_23 ", " #_24 ", " #_25 ", " #_26;
#define QRY_SCRIPT_27(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16 ", " #_17 ", " #_18 ", " #_19 ", " #_20 ", " #_21 ", " #_22 ", " #_23 ", " #_24 ", " #_25 ", " #_26 ", " #_27;
#define QRY_SCRIPT_28(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16 ", " #_17 ", " #_18 ", " #_19 ", " #_20 ", " #_21 ", " #_22 ", " #_23 ", " #_24 ", " #_25 ", " #_26 ", " #_27 ", " #_28;
#define QRY_SCRIPT_29(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16 ", " #_17 ", " #_18 ", " #_19 ", " #_20 ", " #_21 ", " #_22 ", " #_23 ", " #_24 ", " #_25 ", " #_26 ", " #_27 ", " #_28 ", " #_29;
#define QRY_SCRIPT_30(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30) \
    #_1 ", " #_2 ", " #_3 ", " #_4 ", " #_5 ", " #_6 ", " #_7 ", " #_8 ", " #_9 ", " #_10 ", " #_11 ", " #_12 ", " #_13 ", " #_14 ", " #_15 ", " #_16 ", " #_17 ", " #_18 ", " #_19 ", " #_20 ", " #_21 ", " #_22 ", " #_23 ", " #_24 ", " #_25 ", " #_26 ", " #_27 ", " #_28 ", " #_29 ", " #_30;

