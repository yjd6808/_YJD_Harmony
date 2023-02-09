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

bool Directory::Exist(const char* path) {
    DWORD dwAttr = GetFileAttributes(path);

    return (dwAttr != INVALID_FILE_ATTRIBUTES &&
		   (dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}

bool Directory::Exist(const String& path) {
    return Exist(path.Source());
}

bool Directory::Create(const char* path) {
    return ::CreateDirectoryA(path, NULL) != 0;
}

bool Directory::Create(const String& path) {
    return Create(path.Source());
}

bool Directory::Delete(const char* path) {
    return std::filesystem::remove_all(path) != static_cast<uintmax_t>(-1);
}

bool Directory::Delete(const String& path) {
    return Delete(path.Source());
}

Vector<String, DefaultAllocator> Directory::Files(const char* path, bool recursive) {
    Vector<String, DefaultAllocator> result;

    if (recursive) {
        for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(path))
            if (!std::filesystem::is_directory(dirEntry))
                result.PushBack(dirEntry.path().string().c_str());
    } else {
        for (const auto& dirEntry : std::filesystem::directory_iterator(path))
            if (!std::filesystem::is_directory(dirEntry))
                result.PushBack(dirEntry.path().string().c_str());
    }


    return result;
}


void Directory::FilesRecursive(Vector<String>& files, String path, bool recursive) {
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(path.Source(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // 숨겨진 닷 디렉토리가 있네 ㄷㄷ;
            if (fd.cFileName[0] == '.') continue;

            String filePath = StringUtil::Format("%s/%s", path.Source(), fd.cFileName);

            // 디렉토리가 아닌 녀석들
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                files.PushBack(filePath);
                continue;
            }

            if (recursive) {
                FilesRecursive(files, filePath, recursive);
            }

        } while (::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }

}

NS_JC_END
