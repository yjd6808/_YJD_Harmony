/*
 * 작성자: 윤정도
 * 생성일: 5/20/2023 9:54:20 AM
 * =====================
 */

#pragma once

#include <JCore/TypeTraits.h>
#include <JCore/Macro.h>
#include <JCore/Logger/LoggerDefine.h>

#include <SteinsGate/Common/ServerNamespace.h>
#include <SteinsGate/Common/MysqlDatabase.h>
#include <SteinsGate/Common/StatementType.h>
#include <SteinsGate/Common/QueryHelper.h>

NS_QRY_BEGIN
template <typename TQryResult>
struct IsVisitable : JCore::FalseType
{
};

template <typename TQryResult>
struct Visitable
{
	static_assert(IsVisitable<TQryResult>::value, "... T is not visitable Qry Struct");

	constexpr static void BindSelectResult(JCORE_IN TQryResult& _result, MysqlQuerySelect* _pQuery)
	{
	}

	constexpr static int FieldCount = 0;
};


template <typename TQryResult>
constexpr int GetSelectResultFieldCount()
{
	return Visitable<TQryResult>::FieldCount;
}

template <typename TQryResult>
constexpr int GetSelectResultFieldCount(const TQryResult& _qryResult)
{
	return Visitable<TQryResult>::FieldCount;
}

template <typename TQryResultT>
constexpr void BindSelectResult(TQryResultT&& _result, MysqlQuerySelect* _pQuery)
{
	Visitable<JCore::NakedType_t<TQryResultT>>::BindSelectResult(JCore::Forward<TQryResultT>(_result), _pQuery);
}

#define QRY_STRUCT_FIELD_NAMES(...)     JCORE_EVEN_ARGS(__VA_ARGS__)
#define QRY_STRUCT_VARIABLE_NAMES(...)  JCORE_ODD_ARGS(__VA_ARGS__)

// 쿼리 구조체 적용자
#define QRY_STRUCT_APPLY_FN_DEF(pair)                       QRY_STRUCT_APPLY_FN_DEF_(JCORE_EXPAND_ARGS pair)        // (1,2) => 1,2로 벗겨서 전달

#define QRY_STRUCT_APPLY_FN_DEF_(...)                       JCORE_EXPAND_1(QRY_STRUCT_APPLY_FN_DEF__(__VA_ARGS__))  // MSVC는 __VA_ARGS__가 하나의 인자로 취급되므로 확장시켜서 전달

#define QRY_STRUCT_APPLY_FN_DEF__(variableName, fieldName)  SelectResultBinder::BindField(fieldName, _result.variableName, _pQuery);
#define QRY_STRUCT_APPLY(...)                               JCORE_APPLY_FN(QRY_STRUCT_APPLY_FN_DEF, JCORE_GROUP_OF(2, __VA_ARGS__))

// 결과 바인딩이 가능하도록 쿼리 구조체 등록 (무조건 Qry 네임스페이스 내부에서 사용할 것)
#define REGISTER_QRY_STRUCT(struct_name, ...)                                                                               \
                                                                                                                            \
template <>                                                                                                                 \
struct IsVisitable<struct_name> : JCore::TrueType {};                                                                      \
                                                                                                                            \
template<>                                                                                                                  \
struct Visitable<struct_name>                                                                                               \
{                                                                                                                           \
	enum Fields                                                                                                           \
	{                                                                                                                    \
		QRY_STRUCT_VARIABLE_NAMES(__VA_ARGS__),                                                                           \
		Count                                                                                                             \
	};                                                                                                                  \
                                                                                                                            \
	constexpr static void BindSelectResult(JCORE_IN struct_name& _result, MysqlQuerySelect* _pQuery)                      \
	{                                                                                                                    \
		QRY_STRUCT_APPLY(__VA_ARGS__)                                                                                    \
	}                                                                                                                    \
                                                                                                                            \
	constexpr static int FieldCount = Fields::Count;                                                                       \
};

#define QRY_RESULT_DEBUG_ASSERT DebugAssertMsg(Query != nullptr, "쿼리 변수가 NULL임");

struct Result
{
	int LastErrorCode = InvalidValue_v;
	int RowCount{};
	bool Success{};
};

template <typename TQryResult>
struct SelectResult : Result
{
	MysqlQuerySelectPtr Query;
	bool HasBindedResult = false;

	bool HasNext() const
	{
		QRY_RESULT_DEBUG_ASSERT
		if (!Success) return false;
		return Query->HasNext();
	}

	void FetchCurrentRow()
	{
		QRY_RESULT_DEBUG_ASSERT
		TQryResult& result = static_cast<TQryResult&>(*this);
		BindSelectResult(result, Query.GetPtr());
		HasBindedResult = true;
	}

	bool FetchNextRow()
	{
		QRY_RESULT_DEBUG_ASSERT
		if (!Success) return false;

		if (Query->Next())
		{
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
	constexpr static void BindField(const char* _pFieldName, TField& _fieldVariable, MysqlQuerySelect* _pQuery)
	{
		if constexpr (JCore::IsString_v<TField>)
		{
			_fieldVariable = _pQuery->GetRawString(_pFieldName);
		}
		else if constexpr (
			JCore::Or_v<
				JCore::IsIntegerType_v<TField>,
				JCore::IsCharaterType_v<TField>,
				JCore::IsFloatType_v<TField>>)
		{
			_fieldVariable = _pQuery->GetNumber<TField>(_pFieldName);
		}
		else if constexpr (JCore::IsDateTime_v<TField>)
		{
			_fieldVariable = _pQuery->GetDateTime(_pFieldName);
		}
		else
		{
			DebugAssertMsg(false, "바인딩 할 수 없는 필드입니다. (TField = %s)", typeid(TField).name());
		}
	}
};


template <typename TQry>
struct SelectStatement
{
	template <typename THelper, typename TResult, typename... Args>
	constexpr static void Execute(MysqlDatabase* _pDatabase, TResult& _result, Args&&... _args)
	{
		static_assert(IsQryHelper_v<THelper>, "... THelper is not QueryHelper<T>");

		THelper::SetLastErrorCode(InvalidValue_v);
		static_assert(JCore::StringUtil::CTCountChar(TQry::Script, '?') == sizeof...(_args));
		/*if constexpr (JCore::StringUtil::CTCountChar(TQry::Script, '?') != sizeof...(_args)) {
			_LogWarn_("쿼리 스크립트에서 요구하는 인자갯수와 전달받은 인자 갯수가 틀립니다.");
			return;
		}*/

		auto pQuery = _pDatabase->Query(TQry::Script, JCore::Forward<Args>(_args)...);

		if (pQuery == nullptr)
		{
			return;
		}

		DebugAssertMsg(pQuery->GetStatementType() == StatementType::Select, "셀렉트 스테이트먼트가 아닙니다.");

		const int errorCode = pQuery->GetErrorCode();

		_result.Query = pQuery;
		_result.Success = errorCode == 0;
		_result.LastErrorCode = errorCode;
		_result.RowCount = errorCode == 0 ? pQuery->GetRowCount() : 0;

		THelper::SetLastErrorCode(errorCode);

		if (!_result.Query->HasNext())
		{
			return;
		}

		_result.FetchCurrentRow();
	}
};

struct InsertResult : Result
{
	MysqlQueryInsertPtr Query;

	Int64U GetInsertId()
	{
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


template <StatementType StatementType>
struct ResultTyGetter
{
	using Ty = void;
};

template <>
struct ResultTyGetter<StatementType::Insert>
{
	using Ty = InsertResult;
};

template <>
struct ResultTyGetter<StatementType::Delete>
{
	using Ty = DeleteResult;
};

template <>
struct ResultTyGetter<StatementType::Update>
{
	using Ty = UpdateResult;
};


template <StatementType StatementType, typename TQry>
struct Statement
{
	template <typename THelper, typename... Args>
	constexpr static void Execute(MysqlDatabase* _pDatabase, typename ResultTyGetter<StatementType>::Ty& _result,
	                              Args&&... _args)
	{
		static_assert(IsQryHelper_v<THelper>, "... THelper is not QueryHelper<T>");

		THelper::SetLastErrorCode(InvalidValue_v);
		static_assert(JCore::StringUtil::CTCountChar(TQry::Script, '?') == sizeof...(_args));
		/*if constexpr (JCore::StringUtil::CTCountChar(TQry::Script, '?') != sizeof...(_args)) {
			_LogWarn_("쿼리 스크립트에서 요구하는 인자갯수와 전달받은 인자 갯수가 틀립니다.");
			return;
		}*/

		auto pQuery = _pDatabase->Query(TQry::Script, JCore::Forward<Args>(_args)...);

		if (pQuery == nullptr)
		{
			return;
		}

		DebugAssertMsg(pQuery->GetStatementType() == StatementType, "%s 스테이트먼트가 아닙니다.", StatementName(StatementType));
		const int errorCode = pQuery->GetErrorCode();

		_result.Query = pQuery;
		_result.Success = errorCode == 0;
		_result.LastErrorCode = errorCode;
		_result.RowCount = errorCode == 0 ? pQuery->GetRowCount() : 0;

		THelper::SetLastErrorCode(pQuery->GetErrorCode());
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
