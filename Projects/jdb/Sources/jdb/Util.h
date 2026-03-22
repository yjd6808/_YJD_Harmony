/*
 * 작성자: 윤정도
 * 생성일: 3/13/2026
 * =====================
 *
 * jdb 공통 유틸리티
 * {0}, {1}, ... 형태의 위치 기반 플레이스홀더를 치환한다.
 */

#pragma once

#include "jc/Primitives/String.h"

#include "jdb/MySQL/MysqlStatementBuilder.h"
#include "jdb/SQLServer/SqlServerStatementBuilder.h"

#include "jdb/Structure.h"

NS_JDB_BEGIN

class JDB_DLL Util
{
public:
	// constexpr static int STMT_PLACEHOLDER_ERROR_CAN_NOT_FIND_OPEN_BRACE		= -1;
	// constexpr static int STMT_PLACEHOLDER_ERROR_INVALID_FORMAT				= -2;
	// constexpr static int STMT_PLACEHOLDER_ERROR_CAN_NOT_FIND_CLOSE_BRACE		= -3;
	// constexpr static int STMT_PLACEHOLDER_ERROR_TOO_LONG_CONTENT				= -4;
	// constexpr static int STMT_PLACEHOLDER_ERROR_NON_NUMERIC_CONTENT			= -5;
	// constexpr static int STMT_PLACEHOLDER_ERROR_INVALID_NUMBER				= -6;
	// constexpr static int STMT_PLACEHOLDER_ERROR_MISSING_NUMBER				= -7;

	struct PlaceholderInfo
	{
		int errorCode_ = 0;
		int count_ = 0;
		int map_[STMT_PLACEHOLDER_MAX]{};

		static constexpr PlaceholderInfo Error(int _errorCode)
		{
			PlaceholderInfo r;
			r.errorCode_ = _errorCode;
			return r;
		}

		static constexpr PlaceholderInfo None()
		{
			PlaceholderInfo r;
			return r;
		}
	};

	constexpr static PlaceholderInfo CTParsePlaceholder(const char* _pStr)
	{
		// {0}, {1}, {2} ... 이런식으로 포맷팅할 때, 몇 개의 플레이스홀더가 있는지 세는 함수
		// 중간에 비는게 있으면 -1을 반환하여 올바르지 않은 플레이스 홀더를 나타내도록 한다.
		char temp[32]{};
		int i = 0;
		int len = jc::StringUtil::CTLength(_pStr);
		int maxNum = -1;
		PlaceholderInfo r;
		do
		{
			i = jc::StringUtil::CTFindChar(_pStr, '{', i);
			if (i <= -1)
				break; // 더 이상 { 문자가 없는 경우
			if (i + 1 >= len)
				return PlaceholderInfo::Error(-2); // {이 마지막 문자인 경우
			int e = jc::StringUtil::CTFindChar(_pStr, '}', i + 1);
			if (e <= -1)
				return PlaceholderInfo::Error(-3); // { 다음에 } 문자가 없는 경우
			int sz = e - i - 1;
			if (sz >= 32)
				return PlaceholderInfo::Error(-4); // 플레이스홀더 내용이 말도 안되는 경우
			jc::StringUtil::CTCopy(temp, 32, _pStr + i + 1, sz);
			for (int k = 0; k < sz; ++k)
				if (temp[k] < '0' || temp[k] > '9')
					return PlaceholderInfo::Error(-5); // 플레이스홀더 내용이 숫자가 아닌 경우
			int num = jc::StringUtil::CTToInt32(temp);
			if (num < 0 || num >= STMT_PLACEHOLDER_MAX)
				return PlaceholderInfo::Error(-6); // 플레이스홀더 번호가 음수이거나 최대 번호(100)를 넘는 경우
			++r.map_[num]; // 존재하는 플레이스홀더 표시
			if (num > maxNum)
				maxNum = num;
			jc::StringUtil::CTZeroMemory(temp, 32);
			i = e + 1;
		} while (i < len);

		for (int j = 0; j <= maxNum; ++j)
			if (r.map_[j] == 0) // 플레이스홀더 번호가 중간에 비어있는 경우 {0}{1}{3}
				return PlaceholderInfo::Error(-7);
		int count = 0;
		for (int j = 0; j <= maxNum; ++j)
			count += r.map_[j];
		r.count_ = count;
		return r;
	}

	constexpr static StatementType CTParseStmtType(const char* _pStr)
	{
		// 문자열이 SELECT, INSERT, UPDATE, DELETE 중 무엇으로 시작하는지 판단하여 StatementType을 반환한다.
		// 대소문자 구분 없이 판단한다.
		int length = jc::StringUtil::CTLength(_pStr);
		if (jc::StringUtil::CTFind(_pStr, length, "SELECT", 6, 0, false) == 0)
			return StatementType::Select;
		if (jc::StringUtil::CTFind(_pStr, length, "INSERT", 6, 0, false) == 0)
			return StatementType::Insert;
		if (jc::StringUtil::CTFind(_pStr, length, "UPDATE", 6, 0, false) == 0)
			return StatementType::Update;
		if (jc::StringUtil::CTFind(_pStr, length, "DELETE", 6, 0, false) == 0)
			return StatementType::Delete;
		return StatementType::Etc;
	}

	constexpr static StmtTemplate CTParseStmt(const char* _pStr)
	{
		PlaceholderInfo placeholderInfo = CTParsePlaceholder(_pStr);

		StmtTemplate ret;
		ret.text_ = jc::StringView(_pStr);
		ret.type_ = CTParseStmtType(_pStr);
		ret.argCount_ = placeholderInfo.count_;
		return ret;
	}


	template <typename... Args>
	static BoundStmt BuildStmt(DatabaseType _dbType, const StmtTemplate& _template, Args&&... _args)
	{
		jc::String ptmt(0);
		if constexpr (sizeof...(Args) == 0)
		{
			ptmt = _template.text_;
		}
		else
		{
			if (_dbType == DatabaseType::dbtMySQL)
			{
				ptmt = BuildStmtInternal<MysqlStatementBuilder, Args...>(_template, jc::Forward<Args>(_args)...);
			}
			else
			{
				ptmt = BuildStmtInternal<SqlServerStatementBuilder, Args...>(_template, jc::Forward<Args>(_args)...);
			}
		}

		BoundStmt stmt;
		stmt.text_ = Move(ptmt);
		stmt.info_ = _template;
		return stmt;
	}

	static jc::String ReplacePlaceholders(const jc::StringView& _stmt, const jc::String* _pArgs, int _argCount);

private:
	template <typename TBuilder, typename T, typename... Args>
	constexpr static jc::String BuildStmtInternal(const StmtTemplate& _stmt, T&& _arg, Args&&... _args)
	{
		if (!TBuilder::IsInitialized())
		{
			jc_assert_msg(false, "우선 빌더의 Initialize를 호출해주세요");
			return jc::String();
		}

		constexpr int ARG_COUNT = sizeof...(Args) + 1;
		static_assert(ARG_COUNT <= STMT_PLACEHOLDER_MAX, "인자 수가 MAX_ARGS를 초과합니다.");

		if (_stmt.argCount_ != ARG_COUNT)
		{
			jc_assert_msg(false, "인자 수가 플레이스홀더 수와 일치하지 않습니다.");
			return jc::String();
		}

		thread_local jc::String fieldMap[STMT_PLACEHOLDER_MAX];
		TBuilder::Set(fieldMap, 0, jc::Forward<T>(_arg), jc::Forward<Args>(_args)...);
		return jc::String(Util::ReplacePlaceholders(_stmt.text_, fieldMap, ARG_COUNT));
	}

	
};

NS_END
