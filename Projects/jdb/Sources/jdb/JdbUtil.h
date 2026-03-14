/*
 * 작성자: 윤정도
 * 생성일: 3/13/2026
 * =====================
 *
 * jdb 공통 유틸리티
 * {0}, {1}, ... 형태의 위치 기반 플레이스홀더를 치환한다.
 */

#pragma once

#include <jc/Primitives/String.h>

#include <jdb/Namespace.h>
#include <jdb/DLLExport.h>

NS_JDB_BEGIN

class JDB_DLL JdbUtil
{
public:
	static constexpr int MAX_ARGS = 128;

	static jc::String ReplacePlaceholders(const jc::String& _statement, const jc::String* _pArgs, int _argCount);

	template <typename TBuilder, typename T, typename... Args>
	constexpr static jc::String Build(const jc::String& _statement, T&& _arg, Args&&... _args)
	{
		if (!TBuilder::IsInitialized())
		{
			jc_assert_msg(false, "우선 빌더의 Initialize를 호출해주세요");
			return jc::String();
		}

		constexpr int ARG_COUNT = sizeof...(Args) + 1;
		static_assert(ARG_COUNT <= JdbUtil::MAX_ARGS, "인자 수가 MAX_ARGS를 초과합니다.");

		thread_local jc::String fieldMap[JdbUtil::MAX_ARGS];
		TBuilder::Set(fieldMap, 0, jc::Forward<T>(_arg), jc::Forward<Args>(_args)...);
		return jc::String(JdbUtil::ReplacePlaceholders(_statement, fieldMap, ARG_COUNT));
	}
};

NS_END
