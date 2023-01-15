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
		//
		//String szResult{ lhs.Length() + rhs.Length() + 10 };

		//szResult += lhs;
		//lhs.T

		return (std::filesystem::path(lhs.Source()) / rhs.Source()).string().c_str();
	}

	String Path::Combine(const String& lhs, const String& rhs, const String& khs) {
		return (std::filesystem::path(lhs.Source()) / rhs.Source() / khs.Source()).string().c_str();
	}


}
