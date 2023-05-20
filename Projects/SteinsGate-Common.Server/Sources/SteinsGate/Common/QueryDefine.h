#pragma once

#include <JCore/TypeTraits.h>
#include <SteinsGate/Common/MysqlDatabase.h>


template <typename... TFields> 
struct QueryFieldList
{
};

template <typename T, const char* FileName>
struct QueryField
{
	static constexpr const char* Name = FileName;
	T Value;
};

#define QueryFieldBegin()
#define QueryFieldEnd()
#define QueryFieldDef(__type__, __field_name__)									\
	static constexpr char __field_name__[] = #__field_name__;					\
	static constexpr QueryField<int, __field_name__> Field_##__field_name__{};
	



struct QrySelectAccountInfoResult
{
	QrySelectAccountInfoResult()
		: Name(0)
		, Pass(0)
	{}

	SGString Name;
	SGString Pass;
	SGDateTime Login;

	static constexpr const char* FieldName = "id";
	static constexpr const char* FieldPass = "pw";


	/*void BindFields(MysqlQuerySelect* query) {
		Name = query->GetString("efg");
		Pass = query->GetString("abcd");
	}*/
};


template <typename TQry>
struct QryBase
{
	static constexpr int ARGS_COUNT = SGStringUtil::CTCountChar(TQry::SCRIPT, '?');
};

template <typename TQry>
struct QrySelector : QryBase<TQry>
{
	template <typename... Args>
	static bool Execute(MysqlDatabase* database, TQry::TResult& result, Args&&... args) {
		auto spQuery = database->Query(TQry::SCRIPT, JCore::Forward<Args>(args)...);
		result.BindFields(spQuery.GetPtr());
		return true;
	}
};



struct QrySelectAccountInfo : QrySelector<QrySelectAccountInfo>
{
	using TResult = QrySelectAccountInfoResult;
	static constexpr const char* SCRIPT = "select ";
};


