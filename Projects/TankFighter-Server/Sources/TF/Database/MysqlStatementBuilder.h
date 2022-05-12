/*
	�ۼ��� : ������
	statement ����

	����� ��ٷο� Forwarding �����̹Ƿ� ������ �����ؾ��Ѵ�.
*/

#pragma once

#include <map>
#include <string>
#include <sstream>

#include <JCore/Time.h>
#include <JCore/Container/HashMap.h>

#include <TF/Util/Console.h>
#include <TF/Database/MysqlConnection.h>
#include <TF/ServerConfiguration.h>


#ifndef DebugAssert
	#include <cassert>
	#define DebugAssert(exp, msg)		assert((exp) && msg)
#endif


class MysqlStatementBuilder
{
	using TFieldMap = JCore::HashMap<int, JCore::String>;
private:

	// ������ �߰��ϰ� ���� ���� ���������� ���ø� Ư��ȭ ���� ��
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

	// Forward�ÿ��� �ش� Ÿ�� �״�� �Ѱ��ְ�
	// Setter ���ʹ� �⺻ Ÿ�����ιٲ������
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
			DebugAssert(false, "�켱 ������ Initailize�� ȣ�����ּ���");
			return "";
		}

		TFieldMap argMap;
		const int argCount = statement.Count("?");

		if (argCount != sizeof...(args))
			return "";

		// �ʵ� ����ŭ �Ҵ�
		for (int i = 1; i <= argCount; i++)
			argMap.Insert(i, "");

		
		
		if constexpr (sizeof...(Args) > 0) {
			// �� �ʵ庰�� �´� Ÿ�� ����
			// 1��° ���� ��������� �����س�����.
			Set(argMap, 1, JCore::Forward<Args>(args)...);	
		} else {
			// ������Ÿ�ӿ� ���� ���°� ������ - �������� ���� ���� ��� �� �ٷ� �������� ����.
			return statement;
		}
		
		// ���� ����
		int iNextOffset = 0;
		for (int i = 1; i <= argMap.Size(); i++) {
			iNextOffset = statement.Find(iNextOffset, "?");
			iNextOffset = statement.Replace(iNextOffset, 1, argMap[i]);
		}

		return statement;
	}

	static bool Initialize() {
		if (ms_pConn != nullptr)
			return true;

		// mysql_real_escape_string �� �Լ� ����ϱ����ؼ� ��¿������ Ŀ�ؼ� ����ϵ��� ��
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
	inline static MysqlConnection* ms_pConn = nullptr;	// escape�� ����
};
