/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:39:58 AM
 * =====================
 *
 */


#pragma once

NS_JC_BEGIN

template <typename, typename>
class Vector;
class DefaultAllocator;
class String;
class Directory
{
public:
	static bool Exist(const char* path);
	static bool Exist(const String& path);

	static bool Create(const char* path);
	static bool Create(const String& path);

	static bool Delete(const char* path);
	static bool Delete(const String& path);

	static Vector<String, DefaultAllocator> Files(const String& path, bool recursive = true);
	static Vector<String, DefaultAllocator> Files(const char* path, bool recursive = true);

private:
	static void FilesRecursive(Vector<String, DefaultAllocator>& files, String path, bool recursive);
};

NS_JC_END
