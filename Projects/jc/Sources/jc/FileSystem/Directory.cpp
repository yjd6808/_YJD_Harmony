/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:44:18 AM
 * =====================
 *
 */

#include "jc/Container/Vector.h"
#include "jc/FileSystem/Directory.h"

#include "filesystem"
#include "string"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Exist(const char* _pPath)
{
	DWORD attr = GetFileAttributes(_pPath);

	return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Exist(const String& _path)
{
	return Exist(_path.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Create(const char* _pPath)
{
	return std::filesystem::create_directories(std::filesystem::path(_pPath));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Create(const String& _path)
{
	return Create(_path.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Delete(const char* _pPath)
{
	return std::filesystem::remove_all(_pPath) != static_cast<uintmax_t>(-1);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Delete(const String& _path)
{
	return Delete(_path.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
int Directory::DirectoryCount(const String& _path, bool _recursive /* = true */)
{
	return DirectoryCount(_path.Source(), _recursive);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Directory::DirectoryCount(const char* _pPath, bool _recursive /* = true */)
{
	int count = 0;

	if (_recursive)
	{
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(_pPath))
			if (std::filesystem::is_directory(dirEntry))
				count++;
	}
	else
	{
		for (const auto& dirEntry : std::filesystem::directory_iterator(_pPath))
			if (std::filesystem::is_directory(dirEntry))
				count++;
	}

	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Directory::FileCount(const String& _path, bool _recursive /* = true */)
{
	return FileCount(_path.Source(), _recursive);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Directory::FileCount(const char* _pPath, bool _recursive /* = true */)
{
	int count = 0;

	if (_recursive)
	{
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(_pPath))
			if (dirEntry.is_regular_file())
				count++;
	}
	else
	{
		for (const auto& dirEntry : std::filesystem::directory_iterator(_pPath))
			if (dirEntry.is_regular_file())
				count++;
	}

	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String> Directory::Files(const String& _path, bool _recursive /* = true */)
{
	return Files(_path.Source(), _recursive);
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String, CDefaultAllocator> Directory::Files(const char* _pPath, bool _recursive /* = true */)
{
	Vector<String, CDefaultAllocator> result;

	if (_recursive)
	{
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(_pPath))
			if (!std::filesystem::is_directory(dirEntry))
				result.PushBack(dirEntry.path().string().c_str());
	}
	else
	{
		for (const auto& dirEntry : std::filesystem::directory_iterator(_pPath))
			if (!std::filesystem::is_directory(dirEntry))
				result.PushBack(dirEntry.path().string().c_str());
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String> Directory::Directories(const String& _path, bool _recursive)
{
	return Directories(_path.Source(), _recursive);
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String> Directory::Directories(const char* _pPath, bool _recursive)
{
	Vector<String, CDefaultAllocator> result;

	if (_recursive)
	{
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(_pPath))
			if (std::filesystem::is_directory(dirEntry))
				result.PushBack(dirEntry.path().string().c_str());
	}
	else
	{
		for (const auto& dirEntry : std::filesystem::directory_iterator(_pPath))
			if (std::filesystem::is_directory(dirEntry))
				result.PushBack(dirEntry.path().string().c_str());
	}

	return result;
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
			if (findData.cFileName[0] == '.')
				continue;

			String filePath = StringUtil::Format("%s/%s", _path.Source(), findData.cFileName);

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
