/*
	�ۼ��� : ������
	statement ����

	����� ��ٷο� Forwarding �����̹Ƿ� ������ �����ؾ��Ѵ�.
*/

#pragma once

#include <map>
#include <string>
#include <sstream>
#include <mysql.h>

#include <JCore/Time.h>

#include <TF/Util/Console.h>
#include <TF/Database/MysqlConnection.h>
#include <TF/ServerConfiguration.h>


#ifndef DebugAssert
	#include <cassert>
	#define DebugAssert(exp, msg)		assert((exp) && msg)
#endif


class MysqlStatementBuilder
{
	using TFieldMap = std::map<int, std::string>;
private:

	// ������ �߰��ϰ� ���� ���� ���������� ���ø� Ư��ȭ ���� ��
	template <typename T>
	struct Setter;


	template <Int32U Size>
	struct Setter<const char[Size]>
	{
		void operator()(TFieldMap& refArgMap, int idx, const char(str)[Size]) const {
			std::stringstream ss;
			const JCore::String escapedValue = MysqlStatementBuilder::ms_pConn->EscapeString(JCore::String(str));
			ss << "\"" << escapedValue << "\"";
			refArgMap[idx] = ss.str();
		}
	};

	template <Int32U Size>
	struct Setter<char[Size]>
	{
		void operator()(TFieldMap& refArgMap, int idx, char(str)[Size]) const {
			std::stringstream ss;
			const JCore::String escapedValue = MysqlStatementBuilder::ms_pConn->EscapeString(JCore::String(str));
			ss << "\"" << escapedValue << "\"";
			refArgMap[idx] = ss.str();
		}
	};

	template <>
	struct Setter<const char*>
	{
		void operator()(TFieldMap& refArgMap, int idx, const char* val) const {
			std::stringstream ss;
			const JCore::String escapedValue = MysqlStatementBuilder::ms_pConn->EscapeString(JCore::String(val));
			ss << "\"" << escapedValue << "\"";
			refArgMap[idx] = ss.str();
		}
	};

	template <>
	struct Setter<Int64>
	{
		void operator()(TFieldMap& refArgMap, int idx, const Int64 val) const {
			std::stringstream ss;
			ss << val;
			refArgMap[idx] = ss.str();
		}
	};

	template <>
	struct Setter<JCore::String>
	{
		void operator()(TFieldMap& refArgMap , int idx, const JCore::String& val) const {
			std::stringstream ss;
			const JCore::String escapedValue = MysqlStatementBuilder::ms_pConn->EscapeString(val);
			ss << "\"" << escapedValue << "\"";
			refArgMap[idx] = ss.str();
		}
	};

	template <>
	struct Setter<double>
	{
		void operator()(TFieldMap& refArgMap , int idx, const double& val) const {
			std::stringstream ss;
			ss << val;
			refArgMap[idx] = ss.str();
		}
	};


	template <>
	struct Setter<int>
	{
		void operator()(TFieldMap& refArgMap, int idx, const int& val) const {
			std::stringstream ss;
			ss << val;
			refArgMap[idx] = ss.str();
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
	constexpr static JCore::String Build(std::string statement, Args&&... args) {
		if (ms_pConn == nullptr) {
			DebugAssert(false, "�켱 ������ Initailize�� ȣ�����ּ���");
			return "";
		}

		TFieldMap argMap;
		const int argCount = std::count(statement.begin(), statement.end(), '?');

		if (argCount != sizeof...(args))
			return "";

		// �ʵ� ����ŭ �Ҵ�
		for (int i = 1; i <= argCount; i++)
			argMap.insert(std::pair<int, std::string>(i, ""));

		
		
		if constexpr (sizeof...(Args) > 0) {
			// �� �ʵ庰�� �´� Ÿ�� ����
			// 1��° ���� ��������� �����س�����.
			Set(argMap, 1, JCore::Forward<Args>(args)...);	
		} else {
			// ������Ÿ�� ���� - �������� ���� ���� ��� �� �ٷ� �������� ����.
			return statement;
		}
		
		// ���� ����
		for (unsigned int i = 1; i <= argMap.size(); i++) {
			const int iLastFoundPos = statement.find('?');
			statement.replace(iLastFoundPos, 1, "");
			statement.insert(iLastFoundPos, argMap[i]);
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
