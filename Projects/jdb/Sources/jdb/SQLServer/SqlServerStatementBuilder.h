/*
	작성자 : 윤정도
	SQL Server용 statement 빌더

	MySQL StatementBuilder와 동일한 인터페이스를 제공하되,
	SQL Server ODBC 이스케이프 방식을 사용한다.
*/

#pragma once

#include <jc/Time.h>
#include <jc/Container/HashMap.h>
#include "SqlServerConnection.h"

NS_JDB_BEGIN

struct SqlServerDatabaseInfo;
class JDB_DLL SqlServerStatementBuilder
{
	using TFieldMap = jc::HashMap<int, jc::String>;

private:
	template <typename T>
	struct Setter;

	template <_u32 Size>
	struct Setter<const char[Size]>
	{
		void operator()(TFieldMap& _argMap, int _idx, const char* _pStr) const
		{
			_argMap[_idx] = SqlServerStatementBuilder::EscapeValue(jc::String(_pStr));
		}
	};

	template <_u32 Size>
	struct Setter<char[Size]>
	{
		void operator()(TFieldMap& _argMap, int _idx, const char* _pStr) const
		{
			_argMap[_idx] = SqlServerStatementBuilder::EscapeValue(jc::String(_pStr));
		}
	};

	template <>
	struct Setter<const char*>
	{
		void operator()(TFieldMap& _argMap, int _idx, const char* _pStr) const
		{
			_argMap[_idx] = SqlServerStatementBuilder::EscapeValue(jc::String(_pStr));
		}
	};

	template <>
	struct Setter<_s64>
	{
		void operator()(TFieldMap& _argMap, int _idx, const _s64 _val) const
		{
			_argMap[_idx] = _val;
		}
	};

	template <>
	struct Setter<jc::String>
	{
		void operator()(TFieldMap& _argMap, int _idx, const jc::String& _val) const
		{
			_argMap[_idx] = SqlServerStatementBuilder::EscapeValue(_val);
		}

		void operator()(TFieldMap& _argMap, int _idx, jc::String&& _val) const
		{
			_argMap[_idx] = Move(SqlServerStatementBuilder::EscapeValue(_val));
		}
	};

	template <>
	struct Setter<double>
	{
		void operator()(TFieldMap& _argMap, int _idx, const double& _val) const
		{
			_argMap[_idx] = _val;
		}
	};

	template <>
	struct Setter<int>
	{
		void operator()(TFieldMap& _argMap, int _idx, const int& _val) const
		{
			_argMap[_idx] = _val;
		}
	};

	template <>
	struct Setter<jc::DateTime>
	{
		void operator()(TFieldMap& _argMap, int _idx, const jc::DateTime& _val) const
		{
			Setter<jc::String>()(_argMap, _idx, _val.Format("yyyy-MM-dd hh:mm:ss.ffffff"));
		}
	};

	template <>
	struct Setter<std::nullptr_t>
	{
		void operator()(TFieldMap& _argMap, int _idx, const std::nullptr_t& _val) const
		{
			_argMap[_idx] = "NULL";
		}
	};

	template <typename T>
	constexpr static void Set(TFieldMap& _argMap, int _idx, T&& _arg)
	{
		using TNatural = jc::NaturalType_t<T>;

		Setter<TNatural>()(_argMap, _idx, jc::Forward<T>(_arg));
	}

	template <typename T, typename... Args>
	constexpr static void Set(TFieldMap& _argMap, int _idx, T&& _arg, Args&&... _args)
	{
		using TNatural = jc::NaturalType_t<T>;

		Setter<TNatural>()(_argMap, _idx, jc::Forward<T>(_arg));
		Set(_argMap, _idx + 1, jc::Forward<Args>(_args)...);
	}

public:
	// SQL Server 이스케이프: 싱글 쿼트 더블링
	static jc::String EscapeValue(const jc::String& _value)
	{
		jc::String escaped(_value.Length() * 2 + 3);

		escaped += "N'";

		for (int i = 0; i < _value.Length(); ++i)
		{
			char ch = _value[i];
			if (ch == '\'')
				escaped += "''";
			else
				escaped += ch;
		}

		escaped += "'";
		return escaped;
	}

	template <typename... Args>
	constexpr static jc::String Build(jc::String _statement, Args&&... _args)
	{
		if (!ms_initialized)
		{
			jc_assert_msg(false, "우선 빌더의 Initialize를 호출해주세요");
			return "";
		}

		TFieldMap argMap;
		const int argCount = _statement.Count("?");

		if (argCount != sizeof...(Args))
		{
			return "";
		}

		// 필드 수만큼 할당
		for (int fieldIndex = 1; fieldIndex <= argCount; fieldIndex++)
		{
			argMap.Insert(fieldIndex, "");
		}

		if constexpr (sizeof...(Args) > 0)
		{
			Set(argMap, 1, jc::Forward<Args>(_args)...);
		}
		else
		{
			return _statement;
		}

		// 빌드 진행
		int nextOffset = 0;
		for (int fieldIndex = 1; fieldIndex <= argMap.Size(); fieldIndex++)
		{
			nextOffset = _statement.Replace(nextOffset, "?", argMap[fieldIndex]);
		}

		return _statement;
	}

	static bool Initialize(const SqlServerDatabaseInfo& _dbInfo);
	static void Finalize();

private:
	static bool ms_initialized;
};

NS_END
