/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:44:18 AM
 * =====================
 *
 */

#include "jc/Container/Vector.h"
#include "jc/IO/Directory.h"
#include "jc/Primitives/StringConvert.h"

#include "filesystem"
#include "string"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
// 파일시스템 네이티브 경로를 String으로. 경로는 OS 인코딩이라 문자셋 분기가 필요하다.
static String ToStringPath(const std::filesystem::path& _path)
{
#ifdef _UNICODE
	return String(_path.wstring().c_str());
#else
	return String(_path.string().c_str());
#endif
}

bool Directory::Exist(const char* _pPath)
{
	return Exist(StringConvert::FromUtf8(_pPath));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Exist(const String& _path)
{
	DWORD attr = GetFileAttributes(_path.Source());

	return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Create(const char* _pPath)
{
	return Create(StringConvert::FromUtf8(_pPath));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Create(const String& _path)
{
	return std::filesystem::create_directories(std::filesystem::path(_path.Source()));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Delete(const char* _pPath)
{
	return Delete(StringConvert::FromUtf8(_pPath));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Delete(const String& _path)
{
	return std::filesystem::remove_all(std::filesystem::path(_path.Source())) != static_cast<uintmax_t>(-1);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Directory::DirectoryCount(const String& _path, bool _recursive /* = true */)
{
	const std::filesystem::path nativePath(_path.Source());
	int count = 0;

	if (_recursive)
	{
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(nativePath))
			if (std::filesystem::is_directory(dirEntry))
				count++;
	}
	else
	{
		for (const auto& dirEntry : std::filesystem::directory_iterator(nativePath))
			if (std::filesystem::is_directory(dirEntry))
				count++;
	}

	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Directory::DirectoryCount(const char* _pPath, bool _recursive /* = true */)
{
	return DirectoryCount(StringConvert::FromUtf8(_pPath), _recursive);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Directory::FileCount(const String& _path, bool _recursive /* = true */)
{
	const std::filesystem::path nativePath(_path.Source());
	int count = 0;

	if (_recursive)
	{
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(nativePath))
			if (dirEntry.is_regular_file())
				count++;
	}
	else
	{
		for (const auto& dirEntry : std::filesystem::directory_iterator(nativePath))
			if (dirEntry.is_regular_file())
				count++;
	}

	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Directory::FileCount(const char* _pPath, bool _recursive /* = true */)
{
	return FileCount(StringConvert::FromUtf8(_pPath), _recursive);
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String> Directory::Files(const String& _path, bool _recursive /* = true */)
{
	const std::filesystem::path nativePath(_path.Source());
	Vector<String, CDefaultAllocator> result;

	if (_recursive)
	{
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(nativePath))
			if (!std::filesystem::is_directory(dirEntry))
				result.PushBack(ToStringPath(dirEntry.path()));
	}
	else
	{
		for (const auto& dirEntry : std::filesystem::directory_iterator(nativePath))
			if (!std::filesystem::is_directory(dirEntry))
				result.PushBack(ToStringPath(dirEntry.path()));
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String, CDefaultAllocator> Directory::Files(const char* _pPath, bool _recursive /* = true */)
{
	return Files(StringConvert::FromUtf8(_pPath), _recursive);
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String> Directory::Directories(const String& _path, bool _recursive)
{
	const std::filesystem::path nativePath(_path.Source());
	Vector<String, CDefaultAllocator> result;

	if (_recursive)
	{
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(nativePath))
			if (std::filesystem::is_directory(dirEntry))
				result.PushBack(ToStringPath(dirEntry.path()));
	}
	else
	{
		for (const auto& dirEntry : std::filesystem::directory_iterator(nativePath))
			if (std::filesystem::is_directory(dirEntry))
				result.PushBack(ToStringPath(dirEntry.path()));
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String> Directory::Directories(const char* _pPath, bool _recursive)
{
	return Directories(StringConvert::FromUtf8(_pPath), _recursive);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Directory::FilesRecursive(Vector<String>& _files, String _path, bool _recursive)
{
	WIN32_FIND_DATA findData;
	HANDLE hFind = ::FindFirstFile(_path.Source(), &findData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			// 숨겨진 닷 디렉토리가 있네 ㄷㄷ;
			if (findData.cFileName[0] == _T('.'))
				continue;

			String filePath = StringUtilT::Format(_T("%s/%s"), _path.Source(), findData.cFileName);

			// 디렉토리가 아닌 녀석들
			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				_files.PushBack(filePath);
				continue;
			}

			if (_recursive)
			{
				FilesRecursive(_files, filePath, _recursive);
			}

		} while (::FindNextFile(hFind, &findData));

		::FindClose(hFind);
	}
}

NS_END
