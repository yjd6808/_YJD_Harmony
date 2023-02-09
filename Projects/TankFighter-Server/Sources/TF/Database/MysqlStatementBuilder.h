/*
	작성자 : 윤정도
	statement 빌더

	상당히 까다로운 Forwarding 로직이므로 수정시 주의해야한다.
*/

#pragma once

#include <map>
#include <sstream>

#include <JCore/Time.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Utils/Console.h>

#include <TF/Database/MysqlConnection.h>
#include <TF/ServerConfiguration.h>


#ifndef DebugAssertMsg
	#include <cassert>
	#define DebugAssert(exp, msg)		assert((exp) && msg)
#endif


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
		void operator()(TFieldMap& refArgMap, int idx, const char(str)[Size]) const {
			refArgMap[idx] = MysqlStatementBuilder::ms_pConn->EscapeString(JCore::String(str));
		}
	};

	template <Int32U Size>
	struct Setter<char[Size]>
	{
		void operator()(TFieldMap& refArgMap, int idx, char(str)[Size]) const {
			refArgMap[idx] = MysqlStatementBuilder::ms_pConn->EscapeString(JCore::String(str));
		}
	};

	template <>
	struct Setter<const char*>
	{
		void operator()(TFieldMap& refArgMap, int idx, const char* str) const {
			refArgMap[idx] = MysqlStatementBuilder::ms_pConn->EscapeString(JCore::String(str));
		}
	};

	template <>
	struct Setter<Int64>
	{
		void operator()(TFieldMap& refArgMap, int idx, const Int64 val) const {
			refArgMap[idx] = val;
		}
	};

	template <>
	struct Setter<JCore::String>
	{
		void operator()(TFieldMap& refArgMap , int idx, const JCore::String& val) const {
			refArgMap[idx] = MysqlStatementBuilder::ms_pConn->EscapeString(val);
		}

		void operator()(TFieldMap& refArgMap, int idx, JCore::String&& val) const {
			refArgMap[idx] = Move(MysqlStatementBuilder::ms_pConn->EscapeString(val));
		}
	};

	template <>
	struct Setter<double>
	{
		void operator()(TFieldMap& refArgMap , int idx, const double& val) const {
			refArgMap[idx] = val;
		}
	};


	template <>
	struct Setter<int>
	{
		void operator()(TFieldMap& refArgMap, int idx, const int& val) const {
			refArgMap[idx] = val;
		}
	};


	template <>
	struct Setter<JCore::DateTime>
	{
		void operator()(TFieldMap& refArgMap, int idx, const JCore::DateTime& val) const {
			Setter<JCore::String>()(refArgMap, idx, val.Format("yyyy-MM-dd hh:mm:ss.ffffff"));
		}
	};

	template <>
	struct Setter<std::nullptr_t>
	{
		void operator()(TFieldMap& refArgMap, int idx, const std::nullptr_t& val) const {
			refArgMap[idx] = "NULL";
		}
	};

	// Forward시에는 해당 타입 그대로 넘겨주고
	// Setter 펑터는 기본 타입으로바꿔줘야함
	template <typename T>
	constexpr static void Set(TFieldMap& refArgMap, int idx, T&& arg) {
		using TNatural = JCore::NaturalType_t<T>;

		Setter<TNatural>()(refArgMap, idx, JCore::Forward<T>(arg));
	}

	template <typename T, typename... Args>
	constexpr static void Set(TFieldMap& refArgMap, int idx, T&& arg, Args&&... args) {
		using TNatural = JCore::NaturalType_t<T>;

		Setter<TNatural>()(refArgMap, idx, JCore::Forward<T>(arg));
		Set(refArgMap, idx + 1, JCore::Forward<Args>(args)...);
	}

public:

	template <typename... Args>
	constexpr static JCore::String Build(JCore::String statement, Args&&... args) {
		if (ms_pConn == nullptr) {
			DebugAssertMsg(false, "우선 빌더의 Initailize를 호출해주세요");
			return "";
		}

		TFieldMap argMap;
		const int argCount = statement.Count("?");

		if (argCount != sizeof...(args))
			return "";

		// 필드 수만큼 할당
		for (int i = 1; i <= argCount; i++)
			argMap.Insert(i, "");

		if constexpr (sizeof...(Args) > 0) {
			// 각 필드별로 맞는 타입 세팅
			// 1번째 부터 재귀적으로 세팅해나간다.
			Set(argMap, 1, JCore::Forward<Args>(args)...);	
		} else {
			// 컴파일타임에 리턴 상태가 결정댐 - 수정해줄 값이 없을 경우 걍 바로 끝내도록 하자.
			return statement;
		}
		
		// 빌드 진행
		int iNextOffset = 0;
		for (int i = 1; i <= argMap.Size(); i++) {
			iNextOffset = statement.Replace(iNextOffset, "?", argMap[i]);
		}

		return statement;
	}

	static bool Initialize() {
		if (ms_pConn != nullptr)
			return true;

		// mysql_real_escape_string 이 함수 사용하기위해서 어쩔수없이 커넥션 사용하도록 함
		ms_pConn = new MysqlConnection();
		return ms_pConn->Connect(DB_HOST, DB_PORT, DB_ID, DB_PASS, DB_NAME);
	}

	static void Finalize() {
		if (ms_pConn) {
			ms_pConn->Disconnect();
			delete ms_pConn;
			ms_pConn = nullptr;
		}
	}

private:
	inline static MysqlConnection* ms_pConn = nullptr;	// escape용 연결
};
