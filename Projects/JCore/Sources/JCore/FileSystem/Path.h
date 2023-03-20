/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 9:02:12 AM
 * =====================
 *
 */


#pragma once

#include <JCore/TypeCast.h>
#include <JCore/Primitives/String.h>

NS_JC_BEGIN

class Path
{
public:
	static String FileName(const String& path);
	static String FileName(const char* path, int length);
	static String FileName(const char* path);
	static String FileNameWithoutExt(const String& path);
	static String FileNameWithoutExt(const char* path);
	static String FileNameLevel(const String& path, int level);
	static String FileNameLevel(const char* path, int length, int level);
	static String FileNameLevel(const char* path, int level);
	static void FileNameLevel(char* buf, int bufCapacity, const char* path, int pathLen, int level);
	static String Combine(const String& lhs, const String& rhs);
	static String Combine(const String& lhs, const String& rhs, const String& khs);
	static String Extension(const String& path);
};

NS_JC_END