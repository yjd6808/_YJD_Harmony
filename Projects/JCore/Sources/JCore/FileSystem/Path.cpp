/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 9:13:03 AM
 * =====================
 *
 */



#include <JCore/Core.h>
#include <JCore/Memory.h>

#include <JCore/FileSystem/Path.h>


NS_JC_BEGIN

String Path::FileName(const String& path) {
	return FileName(path.Source(), path.Length());
}

String Path::FileName(const char* path) {
	return FileName(path, strlen(path));
}

String Path::FileNameWithoutExt(const String& path)
{
	return FileNameWithoutExt(path.Source());
}

String Path::FileNameWithoutExt(const char* path)
{
	const String szFileName = FileName(path);
	int lastPeriod = szFileName.FindReverse(".");
	return lastPeriod == -1 ? lastPeriod : szFileName.GetRange(0, lastPeriod);
}

String Path::FileName(const char* path, int length) {
	String szFileName;
	bool bSlashFound = false;
	char* pPath = (char*)path;

	for (int i = length - 1; i >= 0; i--) {
		char& ch = pPath[i];
		if (ch == '\\' || ch == '/') {
			szFileName += pPath + i + 1;
			bSlashFound = true;
			break;
		}
	}
	if (bSlashFound == false)
		return path;

	return szFileName;
}

String Path::FileNameLevel(const String& path, int level) {
	return FileNameLevel(path.Source(), path.Length(), level);
}

String Path::FileNameLevel(const char* path, int level) {
	return FileNameLevel(path, strlen(path), level);
}

void Path::FileNameLevel(char* buf, int bufCapacity, const char* path, int pathLen, int level) {

	int iLast = bufCapacity - 1;

	char* pPath = (char*)path;
	int iLevel = 0;

	for (int i = pathLen - 1; i >= 0; i--, iLast--) {
		char& ch = pPath[i];

		if (ch == '\\' || ch == '/') {
			buf[iLast] = '/';

			if (iLevel == level) {
				break;
			}

			// 슬래쉬가 아닌 문자열을 만날때까지 체크
			int j = i - 1;
			while (j >= 0 && (pPath[j] == '\\' || pPath[j] == '/')) {
				j--;
			}

			i = j + 1;
			iLevel++;
		}
		else {
			buf[iLast] = ch;
		}
	}

	//  x x x x x a b c
	//          |
	//        iLast 다 읽고나면 빈 곳에 있기때문

	//  x x x x x a b c
	//            |
	//          iLast 이렇게 옮겨 줘야함

	iLast += 1;	// 문자열이 있는 위치로 다시 옮겨 놓는다.

	// 레벨을 높게 잡아버린 경우 마지막 슬래쉬가 포함될 수가 있다.
	// //a///b//c/d에 레벨 10을 전달하면 /a/b/c/d 이렇게 됨 
	if (buf[iLast] == '/') {
		iLast += 1;
	}

	int iFileNameLength = bufCapacity - iLast;
	Memory::Copy(buf, bufCapacity, buf + iLast, iFileNameLength);
	buf[iFileNameLength] = NULL;

}

String Path::FileNameLevel(const char* path, int length, int level) {

	static constexpr int BufSize = 512;
	DebugAssertMsg(length < BufSize, "경로 길이는 버퍼 사이즈보다 작아야합니다.");

	char szFileName[BufSize]{};
	FileNameLevel(szFileName, BufSize, path, length, level);

	return szFileName;
}


String Path::Combine(const String& lhs, const String& rhs) {

	String szCombined{lhs.Source(), lhs.Length() + rhs.Length() + 1};

	int iLast = -1;
	int iCount = 0;
	for (int i = szCombined.Length() - 1; i >= 0; --i, ++iCount) {
		if (szCombined[i] != '/' && szCombined[i] != '\\') {
			iLast = i;
			break;
		}
	}

	if (iLast != -1)
		szCombined.Clear(iLast + 1, iCount);

	szCombined.Append('/');
	iCount = 0;

	for (iLast = 0; iLast < rhs.Length(); ++iLast, ++iCount) {
		if (rhs.GetAt(iLast) != '/' && rhs.GetAt(iLast) != '\\') {
			break;
		}
	}
	szCombined.Append(rhs.Source() + iCount);
	return szCombined;
}

String Path::Combine(const String& lhs, const String& rhs, const String& khs) {
	return Combine(lhs, Combine(rhs, khs));
}

String Path::Extension(const String& path) {
	int iCount = 0;

	for (int i = path.Length() - 1; i >= 0; --i, ++iCount) {

		if (path.GetAt(i) == '.') {
			return { path.Source() + i, iCount + 1};
		}

		if (path.GetAt(i) == '/' || path.GetAt(i) == '\\') {
			return "";
		}
	}

	return "";
}


}
