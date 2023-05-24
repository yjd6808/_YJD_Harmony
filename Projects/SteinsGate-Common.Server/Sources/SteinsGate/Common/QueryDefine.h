/*
 * 작성자: 윤정도
 * 생성일: 5/20/2023 9:54:20 AM
 * =====================
 *
 * [셀렉트 바인딩 예시]
 * Qry::SelectAccountInfoResult result;
 * Qry::SelectAccountInfo::Execute(CoreGameDB_v, result);
 * 
 * while (result.Next()) {
 * 	   result.FetchRow();
 * }
 *
 */

#pragma once

#include <JCore/TypeTraits.h>
#include <JCore/Macro.h>
#include <JCore/Logger/LoggerDefine.h>

#include <SteinsGate/Common/ServerNamespace.h>
#include <SteinsGate/Common/MysqlDatabase.h>
#include <SteinsGate/Common/StatementType.h>

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


template <typename TQryResult>
struct SelectResult
{
	MysqlQuerySelectPtr Query;

	bool HasNext() const {
		DebugAssertMsg(Query != nullptr, "쿼리 변수가 NULL임");
		return Query->HasNext();
	}

	bool Next() const {
		DebugAssertMsg(Query != nullptr, "쿼리 변수가 NULL임");
		return Query->Next();
	}

	void FetchRow() {
		DebugAssertMsg(Query != nullptr, "쿼리 변수가 NULL임");
		TQryResult& result = static_cast<TQryResult&>(*this);
		BindSelectResult(result, Query.GetPtr());
	}

	int GetRowCount() {
		DebugAssertMsg(Query != nullptr, "쿼리 변수가 NULL임");
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
	template <typename TResult, typename... Args>
	constexpr static void Execute(MysqlDatabase* database, TResult& result, Args&&... args) {

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

		if (result.Query->HasNext()) {
			result.FetchRow();
		}
	}
};


NS_QRY_END

