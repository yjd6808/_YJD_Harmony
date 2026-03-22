/*
	작성자 : 윤정도
	SQL Server용 statement 빌더

	{0}, {1}, ... 형태의 위치 기반 인자를 사용한다.
	같은 인덱스를 여러 번 사용할 수 있다.

	예시:
	  Build("select {0}, {1} from table where {0} = 100", "c_uid", "c_name")
*/

#pragma once

#include "jc/Time.h"
#include "jdb/Util.h"

NS_JDB_BEGIN

struct DatabaseInfo;
class JDB_DLL SqlServerStatementBuilder
{
public:
	template <typename T>
	struct Setter;

	template <_u32 Size>
	struct Setter<const char[Size]>
	{
		void operator()(jc::String* _pArgs, int _idx, const char* _pStr) const
		{
			_pArgs[_idx] = SqlServerStatementBuilder::EscapeValue(jc::String(_pStr));
		}
	};

	template <_u32 Size>
	struct Setter<char[Size]>
	{
		void operator()(jc::String* _pArgs, int _idx, const char* _pStr) const
		{
			_pArgs[_idx] = SqlServerStatementBuilder::EscapeValue(jc::String(_pStr));
		}
	};

	template <>
	struct Setter<const char*>
	{
		void operator()(jc::String* _pArgs, int _idx, const char* _pStr) const
		{
			_pArgs[_idx] = SqlServerStatementBuilder::EscapeValue(jc::String(_pStr));
		}
	};

	template <>
	struct Setter<_s64>
	{
		void operator()(jc::String* _pArgs, int _idx, const _s64 _val) const
		{
			_pArgs[_idx] = _val;
		}
	};

	template <>
	struct Setter<jc::String>
	{
		void operator()(jc::String* _pArgs, int _idx, const jc::String& _val) const
		{
			_pArgs[_idx] = SqlServerStatementBuilder::EscapeValue(_val);
		}

		void operator()(jc::String* _pArgs, int _idx, jc::String&& _val) const
		{
			_pArgs[_idx] = Move(SqlServerStatementBuilder::EscapeValue(_val));
		}
	};

	template <>
	struct Setter<double>
	{
		void operator()(jc::String* _pArgs, int _idx, const double& _val) const
		{
			_pArgs[_idx] = _val;
		}
	};

	template <>
	struct Setter<int>
	{
		void operator()(jc::String* _pArgs, int _idx, const int& _val) const
		{
			_pArgs[_idx] = _val;
		}
	};

	template <>
	struct Setter<jc::DateTime>
	{
		void operator()(jc::String* _pArgs, int _idx, const jc::DateTime& _val) const
		{
			Setter<jc::String>()(_pArgs, _idx, _val.Format("yyyy-MM-dd hh:mm:ss.ffffff"));
		}
	};

	template <>
	struct Setter<std::nullptr_t>
	{
		void operator()(jc::String* _pArgs, int _idx, const std::nullptr_t& _val) const
		{
			_pArgs[_idx] = "NULL";
		}
	};

	template <typename T>
	constexpr static void Set(jc::String(&_fieldMap)[STMT_PLACEHOLDER_MAX], int _idx, T&& _arg)
	{
		using TNatural = jc::NaturalType_t<T>;
		Setter<TNatural>()(_fieldMap, _idx, jc::Forward<T>(_arg));
	}

	template <typename T, typename... Args>
	constexpr static void Set(jc::String (&_fieldMap)[STMT_PLACEHOLDER_MAX], int _idx, T&& _arg, Args&&... _args)
	{
		using TNatural = jc::NaturalType_t<T>;
		Setter<TNatural>()(_fieldMap, _idx, jc::Forward<T>(_arg));
		Set(_fieldMap, _idx + 1, jc::Forward<Args>(_args)...);
	}

public:
	static jc::String EscapeValue(const jc::String& _value)
	{
		thread_local jc::String escaped;
		escaped.Clear();
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

	static bool IsInitialized() { return ms_initialized; }
	static bool Initialize(const DatabaseInfo& _dbInfo);
	static void Finalize();

private:
	static bool ms_initialized;
};

NS_END
