/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:44:18 AM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Container/Vector.h>
#include <JCore/FileSystem/Directory.h>

#include <filesystem>
#include <string>

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Exist(const char* _path)
{
    DWORD dwAttr = GetFileAttributes(_path);

    return (dwAttr != INVALID_FILE_ATTRIBUTES && (dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Exist(const String& _path)
{
    return Exist(_path.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Create(const char* _path)
{
    return ::CreateDirectoryA(_path, NULL) != 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Create(const String& _path)
{
    return Create(_path.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Delete(const char* _path)
{
    return std::filesystem::remove_all(_path) != static_cast<uintmax_t>(-1);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Directory::Delete(const String& _path)
{
    return Delete(_path.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
int Directory::DirectoryCount(const String& _path, bool recursive /* = true */)
{
    return DirectoryCount(_path.Source(), recursive);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Directory::DirectoryCount(const char* _path, bool recursive /* = true */)
{
    int iCount = 0;
    if(recursive)
    {
        for(const auto& dirEntry : std::filesystem::recursive_directory_iterator(_path))
            if(std::filesystem::is_directory(dirEntry))
                iCount++;
    }
    else
    {
        for(const auto& dirEntry : std::filesystem::directory_iterator(_path))
            if(std::filesystem::is_directory(dirEntry))
                iCount++;
    }
    return iCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Directory::FileCount(const String& _path, bool recursive /* = true */)
{
    return FileCount(_path.Source(), recursive);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Directory::FileCount(const char* _path, bool recursive /* = true */)
{
    int iCount = 0;
    if(recursive)
    {
        for(const auto& dirEntry : std::filesystem::recursive_directory_iterator(_path))
            if(dirEntry.is_regular_file())
                iCount++;
    }
    else
    {
        for(const auto& dirEntry : std::filesystem::directory_iterator(_path))
            if(dirEntry.is_regular_file())
                iCount++;
    }
    return iCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String> Directory::Files(const String& _path, bool _recursive /* = true */)
{
    return Files(_path.Source(), _recursive);
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String, CDefaultAllocator> Directory::Files(const char* _path, bool _recursive /* = true */)
{
    Vector<String, CDefaultAllocator> result;

    if(_recursive)
    {
        for(const auto& dirEntry : std::filesystem::recursive_directory_iterator(_path))
            if(!std::filesystem::is_directory(dirEntry))
                result.PushBack(dirEntry.path().string().c_str());
    }
    else
    {
        for(const auto& dirEntry : std::filesystem::directory_iterator(_path))
            if(!std::filesystem::is_directory(dirEntry))
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
Vector<String> Directory::Directories(const char* _path, bool _recursive)
{
    Vector<String, CDefaultAllocator> result;

    if(_recursive)
    {
        for(const auto& dirEntry : std::filesystem::recursive_directory_iterator(_path))
            if(std::filesystem::is_directory(dirEntry))
                result.PushBack(dirEntry.path().string().c_str());
    }
    else
    {
        for(const auto& dirEntry : std::filesystem::directory_iterator(_path))
            if(std::filesystem::is_directory(dirEntry))
                result.PushBack(dirEntry.path().string().c_str());
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Directory::FilesRecursive(Vector<String>& _files, String _path, bool _recursive)
{
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(_path.Source(), &fd);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            // 숨겨진 닷 디렉토리가 있네 ㄷㄷ;
            if(fd.cFileName[0] == '.')
                continue;

            String filePath = StringUtil::Format("%s/%s", _path.Source(), fd.cFileName);

            // 디렉토리가 아닌 녀석들
            if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                _files.PushBack(filePath);
                continue;
            }

            if(_recursive)
            {
                FilesRecursive(_files, filePath, _recursive);
            }

        } while(::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
}

NS_JC_END
