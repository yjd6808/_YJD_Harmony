/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 9:02:12 AM
 * =====================
 *
 */


#pragma once

#include <JCore/TypeCast.h>
#include <JCore/Primitives/String.h>

namespace JCore {

	class Path
	{
	public:
		static String FileName(const String& path);
		static String Combine(const String& lhs, const String& rhs);
		static String Combine(const String& lhs, const String& rhs, const String& khs);
		static String Extension(const String& path);
	};
}