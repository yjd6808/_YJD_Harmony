/*
 * 작성자: 윤정도
 * 생성일: 5/14/2023 2:00:09 PM
 * =====================
 *
 */


#pragma once


#include <JCore/Type.h>
#include <JCore/TypeTraits.h>
#include <SteinsGate/Common/MysqlQuery.h>

inline thread_local Int32U QueryResult_v = 0;

inline Int32U GetLastQueryResultError() {
	return QueryResult_v;
}

struct QueryFieldBinder
{
	template <typename T>
	static bool Bind(MysqlQuerySelect* querySelect, T& source, const char* FieldName) {
		const char* field = querySelect->GetRawString(FieldName);
		if (field == nullptr) {
			return false;
		}

		if constexpr (JCore::IsInt32_v<T>) {
			source = querySelect->GetNumber<T>(FieldName);
		} else if constexpr (JCore::IsInt32_v<T>)
			source = querySelect->GetNumber<T>(FieldName);
		else {
			return false;
		}
	}
};




//
//template <>
//struct QueryField<Int32>
//{
//	using TField = QueryField<Int32>;
//
//	QueryField(Int32 v) : Value(v) {}
//	Int32* operator&() { return &Value; }
//	TField& operator=(const TField& v) { Value = v.Value; return *this; }
//	TField& operator=(const Int32& v) { Value = v; return *this; }
//
//	void Bind(MysqlQuerySelect* select) { Value = select->}
//
//	static constexpr QueryFieldType Type = QueryFieldType::Int32;
//	Int32 Value;
//};
//

