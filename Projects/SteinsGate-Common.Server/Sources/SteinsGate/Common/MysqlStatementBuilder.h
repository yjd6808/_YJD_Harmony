/*
	작성자 : 윤정도
	statement 빌더

	상당히 까다로운 Forwarding 로직이므로 수정시 주의해야한다.
*/

#pragma once

#include <JCore/Time.h>
#include <JCore/Container/HashMap.h>
#include <SteinsGate/Common/MysqlConnection.h>

struct DatabaseInfo;

class MysqlStatementBuilder
{
	using TFieldMap = JCore::HashMap<int, JCore::String>;

private:
	// 각각에 추가하고 싶은 인자 있을때마다 템플릿 특수화 해줄 것
	template <typename T>
	struct Setter;

	template <Int32U Size>
	struct Setter<const char[Size]>
	{
		void operator()(TFieldMap& _argMap, int _idx, const char* _pStr) const
		{
			_argMap[_idx] = MysqlStatementBuilder::ms_pConn->EscapeString(JCore::String(_pStr));
		}
	};

	template <Int32U Size>
	struct Setter<char[Size]>
	{
		void operator()(TFieldMap& _argMap, int _idx, const char* _pStr) const
		{
			_argMap[_idx] = MysqlStatementBuilder::ms_pConn->EscapeString(JCore::String(_pStr));
		}
	};

	template <>
	struct Setter<const char*>
	{
		void operator()(TFieldMap& _argMap, int _idx, const char* _pStr) const
		{
			_argMap[_idx] = MysqlStatementBuilder::ms_pConn->EscapeString(JCore::String(_pStr));
		}
	};

	template <>
	struct Setter<Int64>
	{
		void operator()(TFieldMap& _argMap, int _idx, const Int64 _val) const
		{
			_argMap[_idx] = _val;
		}
	};

	template <>
	struct Setter<JCore::String>
	{
		void operator()(TFieldMap& _argMap, int _idx, const JCore::String& _val) const
		{
			_argMap[_idx] = MysqlStatementBuilder::ms_pConn->EscapeString(_val);
		}

		void operator()(TFieldMap& _argMap, int _idx, JCore::String&& _val) const
		{
			_argMap[_idx] = Move(MysqlStatementBuilder::ms_pConn->EscapeString(_val));
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
	struct Setter<JCore::DateTime>
	{
		void operator()(TFieldMap& _argMap, int _idx, const JCore::DateTime& _val) const
		{
			Setter<JCore::String>()(_argMap, _idx, _val.Format("yyyy-MM-dd hh:mm:ss.ffffff"));
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

	// Forward시에는 해당 타입 그대로 넘겨주고
	// Setter 펑터는 기본 타입으로바꿔줘야함
	template <typename T>
	constexpr static void Set(TFieldMap& _argMap, int _idx, T&& _arg)
	{
		using TNatural = JCore::NaturalType_t<T>;

		Setter<TNatural>()(_argMap, _idx, JCore::Forward<T>(_arg));
	}

	template <typename T, typename... Args>
	constexpr static void Set(TFieldMap& _argMap, int _idx, T&& _arg, Args&&... _args)
	{
		using TNatural = JCore::NaturalType_t<T>;

		Setter<TNatural>()(_argMap, _idx, JCore::Forward<T>(_arg));
		Set(_argMap, _idx + 1, JCore::Forward<Args>(_args)...);
	}

public:
	template <typename... Args>
	constexpr static JCore::String Build(JCore::String _statement, Args&&... _args)
	{
		if (ms_pConn == nullptr)
		{
			DebugAssertMsg(false, "우선 빌더의 Initailize를 호출해주세요");
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
			// 각 필드별로 맞는 타입 세팅
			// 1번째 부터 재귀적으로 세팅해나간다.
			Set(argMap, 1, JCore::Forward<Args>(_args)...);
		}
		else
		{
			// 컴파일타임에 리턴 상태가 결정댐 - 수정해줄 값이 없을 경우 걍 바로 끝내도록 하자.
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

	static bool Initialize(const DatabaseInfo* _pDbInfo);
	static void Finalize();

private:
	static MysqlConnection* ms_pConn;
};
