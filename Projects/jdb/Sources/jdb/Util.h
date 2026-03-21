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

#include "jdb/Namespace.h"
#include "jdb/DLLExport.h"

NS_JDB_BEGIN

class JDB_DLL Util
{
public:
	static constexpr int MAX_ARGS = 128;

	static jc::String ReplacePlaceholders(const jc::String& _stmt, const jc::String* _pArgs, int _argCount);
};

NS_END
