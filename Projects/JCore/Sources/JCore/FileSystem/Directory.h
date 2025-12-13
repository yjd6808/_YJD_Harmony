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
class CDefaultAllocator;
class String;
class Directory
{
public:
	static bool Exist(const char* _path);
	static bool Exist(const String& _path);

	static bool Create(const char* _path);
	static bool Create(const String& _path);

	static bool Delete(const char* _path);
	static bool Delete(const String& _path);

	static int DirectoryCount(const String& _path, bool recursive = true);
	static int DirectoryCount(const char* _path, bool recursive = true);

	static int FileCount(const String& _path, bool recursive = true);
	static int FileCount(const char* _path, bool recursive = true);

	static Vector<String, CDefaultAllocator> Files(const String& _path, bool _recursive = true);
	static Vector<String, CDefaultAllocator> Files(const char* _path, bool _recursive = true);

	static Vector<String, CDefaultAllocator> Directories(const String& _path, bool _recursive = true);
	static Vector<String, CDefaultAllocator> Directories(const char* _path, bool _recursive = true);

private:
	static void FilesRecursive(Vector<String, CDefaultAllocator>& _files, String _path, bool _recursive);
};

NS_JC_END
