/*
 * 작성자: 윤정도
 * 생성일: 5/20/2023 9:54:20 AM
 * =====================
 *
 * 리팩토링: IQuery 추상 인터페이스 기반 통합
 * MySQL/SQLServer 구분 없이 하나의 매크로/타입으로 사용 가능
 */



#pragma once

#include <jc/TypeTraits.h>
#include <jc/Macro.h>
#include <jc/Logger/LoggerDefine.h>

#include "IQuery.h"
#include "IDatabase.h"
#include "StatementType.h"
#include "QueryHelper.h"

NS_JDB_BEGIN

// ===========================================================================================
// 공통 SelectResultBinder: IQuerySelect 가상함수 디스패치로 DB 종류 무관하게 동작
// ===========================================================================================

struct SelectResultBinder
{
	template <typename TField>
	constexpr static void BindField(const char* _fieldName, TField& _fieldVariable, IQuery* _executedQuery)
	{
		if constexpr (jc::IsString_v<TField>)
		{
			_fieldVariable = _executedQuery->GetRawString(_fieldName);
		}
		else if constexpr (
			jc::Or_v<
			jc::IsIntegerType_v<TField>,
			jc::IsCharaterType_v<TField>,
			jc::IsFloatType_v<TField>
			>)
		{
			_fieldVariable = _executedQuery->GetNumber<TField>(_fieldName);
		}
		else if constexpr (jc::IsDateTime_v<TField>)
		{
			_fieldVariable = _executedQuery->GetDateTime(_fieldName);
		}
		else
		{
			jc_assert_msg(false, "바인딩 할 수 없는 필드입니다. (TField = %s)", typeid(TField).name());
		}
	}

	template <typename TField>
	constexpr static void BindField(int _fieldIndex, TField& _fieldVariable, IQuery* _executedQuery)
	{
		if constexpr (jc::IsString_v<TField>)
		{
			_fieldVariable = _executedQuery->GetRawString(_fieldIndex);
		}
		else if constexpr (
			jc::Or_v<
			jc::IsIntegerType_v<TField>,
			jc::IsCharaterType_v<TField>,
			jc::IsFloatType_v<TField>
			>)
		{
			_fieldVariable = _executedQuery->GetNumber<TField>(_fieldIndex);
		}
		else if constexpr (jc::IsDateTime_v<TField>)
		{
			_fieldVariable = _executedQuery->GetDateTime(_fieldIndex);
		}
		else
		{
			jc_assert_msg(false, "바인딩 할 수 없는 필드입니다. (TField = %s)", typeid(TField).name());
		}
	}
};

// ===========================================================================================
// 공통 Visitable
// ===========================================================================================

template <typename TQryResult>
struct IsVisitable : jc::FalseType {};

template <typename TQryResult>
struct Visitable
{
	static_assert(IsVisitable<TQryResult>::value, "... T is not visitable Qry Struct");
	constexpr static void BindSelectResult(TQryResult& _v, IQuery* _pQuery) {}
	constexpr static int FIELD_COUNT = 0;
};

template <typename TQryResult>
constexpr int GetSelectResultFieldCount()
{
	return Visitable<TQryResult>::FIELD_COUNT;
}

template <typename TQryResult>
constexpr int GetSelectResultFieldCount(const TQryResult& _v)
{
	return Visitable<TQryResult>::FIELD_COUNT;
}

template <typename TQryResultT>
constexpr void BindSelectResult(TQryResultT&& _v, IQuery* _query)
{
	Visitable<jc::NakedType_t<TQryResultT>>::BindSelectResult(jc::Forward<TQryResultT>(_v), _query);
}

// ===========================================================================================
// 쿼리 구조체 등록 매크로 (DB 종류 무관 통합)
// ===========================================================================================

#define QRY_STRUCT_FIELD_NAMES(...)		JC_EVEN_ARGS(__VA_ARGS__)
#define QRY_STRUCT_VARIABLE_NAMES(...)	JC_ODD_ARGS(__VA_ARGS__)

#define QRY_STRUCT_APPLY_FN_DEF(pair)						QRY_STRUCT_APPLY_FN_DEF_(JC_EXPAND_ARGS pair)
#define QRY_STRUCT_APPLY_FN_DEF_(...)						JC_EXPAND_1(QRY_STRUCT_APPLY_FN_DEF__(__VA_ARGS__))
#define QRY_STRUCT_APPLY_FN_DEF__(variableName, fieldName)	jdb::SelectResultBinder::BindField(fieldName, v.variableName, query);
#define QRY_STRUCT_APPLY(...) JC_APPLY_FN(QRY_STRUCT_APPLY_FN_DEF, JC_GROUP_OF(2, __VA_ARGS__))

// MySQL/SQLServer 공통 쿼리 구조체 등록
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
	constexpr static void BindSelectResult(IN struct_name& v, jdb::IQuery* query) {									\
		QRY_STRUCT_APPLY(__VA_ARGS__)																					\
	}																													\
																														\
	constexpr static int FIELD_COUNT = Fields::Count;																	\
};																														\
}

// 하위 호환용 별칭
#define REGISTER_SQLSERVER_QRY_STRUCT REGISTER_QRY_STRUCT

// ===========================================================================================
// 공통 Result
// ===========================================================================================

#define QRY_RESULT_DEBUG_ASSERT jc_assert_msg(Query != nullptr, "쿼리 변수가 NULL임");

struct Result
{
	int LastErrorCode = -1;
	int RowCount{};
	bool Success{};
};

// ===========================================================================================
// 공통 SelectResult (IQuerySelect 기반)
// ===========================================================================================

template <typename TQryResult>
struct SelectResult : Result
{
	IQueryPtr Query;
	bool HasBindedResult = false;

	IQuery* AsSelect() const
	{
		return Query.GetPtr();
	}

	bool HasNext() const
	{
		QRY_RESULT_DEBUG_ASSERT
		if (!Success)
			return false;
		return AsSelect()->HasNext();
	}

	void FetchCurrentRow()
	{
		QRY_RESULT_DEBUG_ASSERT
		TQryResult& result = static_cast<TQryResult&>(*this);
		BindSelectResult(result, AsSelect());
		HasBindedResult = true;
	}

	bool FetchNextRow()
	{
		QRY_RESULT_DEBUG_ASSERT
		if (!Success)
			return false;

		if (AsSelect()->Next())
		{
			TQryResult& result = static_cast<TQryResult&>(*this);
			BindSelectResult(result, AsSelect());
			HasBindedResult = true;
			return true;
		}

		HasBindedResult = false;
		return false;
	}
};

// ===========================================================================================
// 공통 SelectStatement (Database 타입에 대해 동작)
// ===========================================================================================

template <typename TDatabase, typename TQry>
struct SelectStatement
{
	template <typename THelper, typename TResult, typename... Args>
	constexpr static void Execute(TDatabase* _database, TResult& _result, Args&&... _args)
	{
		static_assert(IsQryHelper_v<THelper>, "... THelper is not QueryHelper<T>");

		THelper::SetLastErrorCode(-1);

		auto spQuery = _database->Query(TQry::Script, jc::Forward<Args>(_args)...);

		if (spQuery == nullptr)
			return;

		jc_assert_msg(spQuery->GetStatementType() == StatementType::Select, "셀렉트 스테이트먼트가 아닙니다.");

		const int iErrorCode = spQuery->GetErrorCode();

		_result.Query = spQuery;
		_result.Success = iErrorCode == 0;
		_result.LastErrorCode = iErrorCode;
		_result.RowCount = iErrorCode == 0 ? spQuery->GetRowCount() : 0;

		THelper::SetLastErrorCode(iErrorCode);

		if (!_result.AsSelect()->HasNext())
			return;

		_result.FetchCurrentRow();
	}
};

// ===========================================================================================
// 공통 Insert/Delete/Update Result
// ===========================================================================================

struct InsertResult : Result
{
	IQueryPtr Query;

	_u64 GetInsertId()
	{
		QRY_RESULT_DEBUG_ASSERT
		if (Query->IsFailed())
			return 0;
		return Query->GetInsertId();
	}
};

struct DeleteResult : Result
{
	IQueryPtr Query;
};

struct UpdateResult : Result
{
	IQueryPtr Query;
};

// ===========================================================================================
// 공통 ResultTyGetter
// ===========================================================================================

template <StatementType ST> struct ResultTyGetter { using Ty = void; };
template <> struct ResultTyGetter<StatementType::Insert> { using Ty = InsertResult; };
template <> struct ResultTyGetter<StatementType::Delete> { using Ty = DeleteResult; };
template <> struct ResultTyGetter<StatementType::Update> { using Ty = UpdateResult; };

// ===========================================================================================
// 공통 Statement (Insert/Update/Delete)
// ===========================================================================================

template <typename TDatabase, StatementType ST, typename TQry>
struct Statement
{
	template <typename THelper, typename... Args>
	constexpr static void Execute(TDatabase* _database, typename ResultTyGetter<ST>::Ty& _result, Args&&... _args)
	{
		static_assert(IsQryHelper_v<THelper>, "... THelper is not QueryHelper<T>");

		THelper::SetLastErrorCode(-1);

		auto spQuery = _database->Query(TQry::Script, jc::Forward<Args>(_args)...);

		if (spQuery == nullptr)
			return;

		jc_assert_msg(spQuery->GetStatementType() == ST, "%s 스테이트먼트가 아닙니다.", StatementName(ST));
		const int iErrorCode = spQuery->GetErrorCode();

		_result.Query = spQuery;
		_result.Success = iErrorCode == 0;
		_result.LastErrorCode = iErrorCode;
		_result.RowCount = iErrorCode == 0 ? spQuery->GetRowCount() : 0;

		THelper::SetLastErrorCode(spQuery->GetErrorCode());
	}
};

NS_END

// ===========================================================================================
// 통합 매크로 (Database 기반 - MySQL/SQLServer 공통)
// ===========================================================================================

#define QRY_SELECT_STATEMENT_BEGIN(struct_name) struct struct_name : jdb::SelectStatement<jdb::Database, struct_name> {
#define QRY_SELECT_STATEMENT_END };

#define QRY_SELECT_RESULT_BEGIN(struct_name) struct struct_name : jdb::SelectResult<struct_name> {
#define QRY_SELECT_RESULT_END };

#define QRY_INSERT_STATEMENT_BEGIN(struct_name) struct struct_name : jdb::Statement<jdb::Database, jdb::StatementType::Insert, struct_name> {
#define QRY_INSERT_STATEMENT_END };

#define QRY_DELETE_STATEMENT_BEGIN(struct_name) struct struct_name : jdb::Statement<jdb::Database, jdb::StatementType::Delete, struct_name> {
#define QRY_DELETE_STATEMENT_END };

#define QRY_UPDATE_STATEMENT_BEGIN(struct_name) struct struct_name : jdb::Statement<jdb::Database, jdb::StatementType::Update, struct_name> {
#define QRY_UPDATE_STATEMENT_END };
