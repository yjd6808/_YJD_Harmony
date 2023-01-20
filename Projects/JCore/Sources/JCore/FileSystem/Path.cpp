/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 9:13:03 AM
 * =====================
 *
 */



#include <JCore/Core.h>
#include <JCore/FileSystem/Path.h>
#include <filesystem>

namespace JCore {

	String Path::FileName(const String& path) {
		String szFileName;

		for (int i = path.Length() - 1; i >= 0; i--) {
			char& ch = path[i];
			if (ch == '\\' || ch == '/') {
				szFileName += path.Source() + i + 1;
				break;
			}
		}
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
		int iLast = 0;
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
