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
	enum Fields { QRY_STRUCT_VARIABLE_NAMES(__VA_ARGS__), Count };														\
																														\
																														\
	constexpr static void BindSelectResult(JCORE_IN struct_name& v,  MysqlQuerySelect* query) {							\
		QRY_STRUCT_APPLY(__VA_ARGS__)																					\
	}																													\
																														\
	constexpr static int FieldCount = Fields::Count;																	\
};																														\

#define QRY_RESULT_DEBUG_ASSERT DebugAssertMsg(Query != nullptr, "쿼리 변수가 NULL임");

template <typename TQryResult>
struct SelectResult
{
	MysqlQuerySelectPtr Query;
	bool HasBindedResult = false;
	bool Success = false;

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

	int GetRowCount() {
		QRY_RESULT_DEBUG_ASSERT
		if (!Success) return 0;
		return Query->GetRowCount();
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

		THelper::SetLastErrorCode(InvalidValue_v);

		if constexpr (SGStringUtil::CTCountChar(TQry::Script, '?') != sizeof...(args)) {
			_LogWarn_("쿼리 스크립트에서 요구하는 인자갯수와 전달받은 인자 갯수가 틀립니다.");
			return;
		}

		auto spQuery = database->Query(TQry::Script, JCore::Forward<Args>(args)...);

		if (spQuery == nullptr) {
			return;
		}

		DebugAssertMsg(spQuery->GetStatementType() == StatementType::Select, "셀렉트 스테이트먼트가 아닙니다.");

		result.Query = spQuery;
		THelper::SetLastErrorCode(spQuery->GetErrorCode());

		if (!result.Query->HasNext()) {
			return;
		}

		result.FetchCurrentRow();
	}
};

struct InsertResult
{
	MysqlQueryInsertPtr Query;

	Int64U GetInsertId() {
		QRY_RESULT_DEBUG_ASSERT
		if (Query->IsFailed()) return 0;
		return Query->GetInsertId();
	}
};

template <typename TQry>
struct InsertStatement
{
	template <typename THelper, typename... Args>
	constexpr static void Execute(MysqlDatabase* database, InsertResult& result, Args&&... args) {
		static_assert(IsQryHelper_v<THelper>, "... THelper is not QueryHelper<T>");

		THelper::SetLastErrorCode(InvalidValue_v);

		if constexpr (SGStringUtil::CTCountChar(TQry::Script, '?') != sizeof...(args)) {
			_LogWarn_("쿼리 스크립트에서 요구하는 인자갯수와 전달받은 인자 갯수가 틀립니다.");
			return;
		}

		auto spQuery = database->Query(TQry::Script, JCore::Forward<Args>(args)...);

		if (spQuery == nullptr) {
			return;
		}

		DebugAssertMsg(spQuery->GetStatementType() == StatementType::Insert, "인설트 스테이트먼트가 아닙니다.");
		result.Query = spQuery;

		THelper::SetLastErrorCode(spQuery->GetErrorCode());
	}
};

struct DeleteResult
{
	MysqlQueryInsertPtr Query;
};


struct UpdateResult
{
	MysqlQueryUpdatePtr Query;
};




NS_QRY_END

#define QRY_SELECT_STATEMENT_BEGIN(struct_name) struct struct_name : SelectStatement<struct_name> {
#define QRY_SELECT_STATEMENT_END(struct_name) };

#define QRY_SELECT_RESULT_BEGIN(struct_name) struct struct_name : SelectResult<struct_name> {
#define QRY_SELECT_RESULT_END(struct_name) };


#define QRY_INSERT_STATEMENT_BEGIN(struct_name) struct struct_name : InsertStatement<struct_name> {
#define QRY_INSERT_STATEMENT_END(struct_name) };

#define QRY_DELETE_STATEMENT_BEGIN(struct_name) struct struct_name : DeleteStatement<struct_name> {
#define QRY_DELETE_STATEMENT_END(struct_name) };

#define QRY_UPDATE_STATEMENT_BEGIN(struct_name) struct struct_name : UpdateStatement<struct_name> {
#define QRY_UPDATE_STATEMENT_END(struct_name) };