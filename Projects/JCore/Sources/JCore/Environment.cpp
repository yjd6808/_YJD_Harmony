/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 10:53:21 PM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Environment.h>
#include <JCore/Primitives/String.h>
#include <JCore/Wrapper/WinApi.h>


namespace JCore {

	String Environment::CurrentDirectory() {
		char szFileName[MAX_PATH];
		int iLen = WinApi::GetModuleFilePath(NULL, szFileName, MAX_PATH);

		while (szFileName[--iLen] != '\\') {}
		if (iLen > 0) szFileName[iLen] = NULL;
		return szFileName;
	}

} // namespace JCore