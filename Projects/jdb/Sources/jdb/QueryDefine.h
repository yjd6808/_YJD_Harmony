/*
 * 작성자: 윤정도
 * 생성일: 5/20/2023 9:54:20 AM
 * =====================
 */



#pragma once

#include <jc/TypeTraits.h>
#include <jc/Macro.h>
#include <jc/Logger/LoggerDefine.h>

#include "MySQL/MysqlDatabase.h"
#include "StatementType.h"
#include "QueryHelper.h"

NS_JDB_BEGIN

template <typename TQryResult>
struct IsVisitable : jc::FalseType {};

template <typename TQryResult>
struct Visitable
{
	static_assert(IsVisitable<TQryResult>::value, "... T is not visitable Qry Struct");
	constexpr static void BindSelectResult(TQryResult& _v, MysqlQuerySelect* _pQuery) {}
	constexpr static int FIELD_COUNT = 0;
};


template <typename TQryResult>
constexpr int GetSelectResultFieldCount() {
	return Visitable<TQryResult>::FIELD_COUNT;
}

template <typename TQryResult>
constexpr int GetSelectResultFieldCount(const TQryResult& _v) {
	return Visitable<TQryResult>::FIELD_COUNT;
}

template <typename TQryResultT>
constexpr void BindSelectResult(TQryResultT&& _v, MysqlQuerySelect* _query) 
{
	Visitable<jc::NakedType_t<TQryResultT>>::BindSelectResult(jc::Forward<TQryResultT>(_v), _query);
}

#define QRY_STRUCT_FIELD_NAMES(...)		JC_EVEN_ARGS(__VA_ARGS__)
#define QRY_STRUCT_VARIABLE_NAMES(...)	JC_ODD_ARGS(__VA_ARGS__)

// 쿼리 구조체 적용자
#define QRY_STRUCT_APPLY_FN_DEF(pair)						QRY_STRUCT_APPLY_FN_DEF_(JC_EXPAND_ARGS pair)		// (1,2) => 1,2로 벗겨서 전달
#define QRY_STRUCT_APPLY_FN_DEF_(...)						JC_EXPAND_1(QRY_STRUCT_APPLY_FN_DEF__(__VA_ARGS__))	// MSVC는 __VA_ARGS__가 하나의 인자로 취급되므로 확장시켜서 전달
#define QRY_STRUCT_APPLY_FN_DEF__(variableName, fieldName)	SelectResultBinder::BindField(fieldName, v.variableName, query);
#define QRY_STRUCT_APPLY(...) JC_APPLY_FN(QRY_STRUCT_APPLY_FN_DEF, JC_GROUP_OF(2, __VA_ARGS__))

// 결과 바인딩이 가능하도록 쿼리 구조체 등록 (무조건 Qry 네임스페이스 내부에서 사용할 것)
#define REGISTER_QRY_STRUCT(struct_name, ...)																			\
namespace jdb {																											\
template <>																												\
struct IsVisitable<struct_name> : jc::TrueType {};																		\
																														\
template<>																												\
struct Visitable<struct_name>																							\
{																														\
	enum Fields { QRY_STRUCT_VARIABLE_NAMES(__VA_ARGS__), Count };														\
																														\
																														\
	constexpr static void BindSelectResult(IN struct_name& v,  jdb::MysqlQuerySelect* query) {							\
		QRY_STRUCT_APPLY(__VA_ARGS__)																					\
	}																													\
																														\
	constexpr static int FieldCount = Fields::Count;																	\
};																														\
}

#define QRY_RESULT_DEBUG_ASSERT jc_assert_msg(Query != nullptr, "쿼리 변수가 NULL임");

struct Result
{
	int LastErrorCode = -1;
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
	constexpr static void BindField(const char* _fieldName, TField& _fieldVariable, MysqlQuerySelect* _executedQuery) {
		if constexpr (jc::IsString_v<TField>) {
			_fieldVariable = _executedQuery->GetRawString(_fieldName);
		}
		else if constexpr (
			jc::Or_v<
			jc::IsIntegerType_v<TField>,
			jc::IsCharaterType_v<TField>,
			jc::IsFloatType_v<TField>
			>) {
			_fieldVariable = _executedQuery->GetNumber<TField>(_fieldName);
		}
		else if constexpr (jc::IsDateTime_v<TField>) {
			_fieldVariable = _executedQuery->GetDateTime(_fieldName);
		}
		else {
			jc_assert_msg(false, "바인딩 할 수 없는 필드입니다. (TField = %s)", typeid(TField).name());
		}
	}
};

template <typename TQry>
struct SelectStatement
{
	template <typename THelper, typename TResult, typename... Args>
	constexpr static void Execute(MysqlDatabase* _database, TResult& _result, Args&&... _args) {
		static_assert(IsQryHelper_v<THelper>, "... THelper is not QueryHelper<T>");

		THelper::SetLastErrorCode(-1);
		static_assert(jc::StringUtil::CTCountChar(TQry::Script, '?') == sizeof...(_args));
		/*if constexpr (jc::StringUtil::CTCountChar(TQry::Script, '?') != sizeof...(args)) {
			_LogWarn_("쿼리 스크립트에서 요구하는 인자갯수와 전달받은 인자 갯수가 틀립니다.");
			return;
		}*/

		auto spQuery = _database->Query(TQry::Script, jc::Forward<Args>(_args)...);

		if (spQuery == nullptr) {
			return;
		}

		jc_assert_msg(spQuery->GetStatementType() == StatementType::Select, "셀렉트 스테이트먼트가 아닙니다.");

		const int iErrorCode = spQuery->GetErrorCode();

		_result.Query = spQuery;
		_result.Success = iErrorCode == 0;
		_result.LastErrorCode = iErrorCode;
		_result.RowCount = iErrorCode == 0 ? spQuery->GetRowCount() : 0;

		THelper::SetLastErrorCode(iErrorCode);

		if (!_result.Query->HasNext()) {
			return;
		}

		_result.FetchCurrentRow();
	}
};

struct InsertResult : Result
{
	MysqlQueryInsertPtr Query;

	_u64 GetInsertId() {
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


template <StatementType StatementType> struct ResultTyGetter { using Ty = void; };
template <> struct ResultTyGetter<StatementType::Insert> { using Ty = InsertResult; };
template <> struct ResultTyGetter<StatementType::Delete> { using Ty = DeleteResult; };
template <> struct ResultTyGetter<StatementType::Update> { using Ty = UpdateResult; };


template <StatementType StatementType, typename TQry>
struct Statement
{
	template <typename THelper, typename... Args>
	constexpr static void Execute(MysqlDatabase* _database, typename ResultTyGetter<StatementType>::Ty& _result, Args&&... _args) 
	{
		static_assert(IsQryHelper_v<THelper>, "... THelper is not QueryHelper<T>");

		THelper::SetLastErrorCode(-1);
		static_assert(jc::StringUtil::CTCountChar(TQry::Script, '?') == sizeof...(_args));
		/*if constexpr (jc::StringUtil::CTCountChar(TQry::Script, '?') != sizeof...(args)) {
			_LogWarn_("쿼리 스크립트에서 요구하는 인자갯수와 전달받은 인자 갯수가 틀립니다.");
			return;
		}*/

		auto spQuery = _database->Query(TQry::Script, jc::Forward<Args>(_args)...);

		if (spQuery == nullptr) {
			return;
		}

		jc_assert_msg(spQuery->GetStatementType() == StatementType, "%s 스테이트먼트가 아닙니다.", StatementName(StatementType));
		const int iErrorCode = spQuery->GetErrorCode();

		_result.Query = spQuery;
		_result.Success = iErrorCode == 0;
		_result.LastErrorCode = iErrorCode;
		_result.RowCount = iErrorCode == 0 ? spQuery->GetRowCount() : 0;

		THelper::SetLastErrorCode(spQuery->GetErrorCode());
	}
};

NS_END

#define QRY_SELECT_STATEMENT_BEGIN(struct_name) struct struct_name : jdb::SelectStatement<struct_name> {
#define QRY_SELECT_STATEMENT_END };

#define QRY_SELECT_RESULT_BEGIN(struct_name) struct struct_name : jdb::SelectResult<struct_name> {
#define QRY_SELECT_RESULT_END };


#define QRY_INSERT_STATEMENT_BEGIN(struct_name) struct struct_name : jdb::Statement<jdb::StatementType::Insert, struct_name> {
#define QRY_INSERT_STATEMENT_END };

#define QRY_DELETE_STATEMENT_BEGIN(struct_name) struct struct_name : jdb::Statement<jdb::StatementType::Delete, struct_name> {
#define QRY_DELETE_STATEMENT_END };

#define QRY_UPDATE_STATEMENT_BEGIN(struct_name) struct struct_name : jdb::Statement<jdb::StatementType::Update, struct_name> {
#define QRY_UPDATE_STATEMENT_END };