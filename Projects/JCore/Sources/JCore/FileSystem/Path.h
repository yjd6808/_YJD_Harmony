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
	static String FileName(const String& _path);
	static String FileName(const char* _pPath, int _length);
	static String FileName(const char* _pPath);
	static String FileNameWithoutExt(const String& _path);
	static String FileNameWithoutExt(const char* _pPath);
	static String FileNameLevel(const String& _path, int _level);
	static String FileNameLevel(const char* _pPath, int _length, int _level);
	static String FileNameLevel(const char* _pPath, int _level);
	static void FileNameLevel(char* _pBuf, int _bufCapacity, const char* _pPath, int _pathLen, int _level);
	static String Combine(const String& _lhs, const String& _rhs);
	static String Combine(const String& _lhs, const String& _rhs, const String& _khs);
	static String Extension(const String& _path);
};

NS_JC_END
