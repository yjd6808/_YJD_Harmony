/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 2:38:08 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGFontLoader.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGJson.h>

#include <JCore/FileSystem/Directory.h>
#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>


USING_NS_JC;
USING_NS_JS;

bool SGFontLoader::LoadFontInfo(
	SGHashMap<SGString, int>& fontNameToCodeMap,
	SGHashMap<int, SGString>& fontCodeToNameMap)
{
	SGString szFontDirPath = JCore::Path::Combine(DataDirectoryPath_v, FontDirectoryName_v);
	Vector<SGString> vFontFiles = Directory::Files(szFontDirPath.Source());

	for (int i = 0; i < vFontFiles.Size(); ++i) {
		SGString& fontPath = vFontFiles[i];
		SGString fontFileName = JCore::Path::FileName(fontPath);

		fontNameToCodeMap.Insert(fontFileName, i);
		fontCodeToNameMap.Insert(i, Move(fontFileName));	// 버리기 아까우니 옴긴다.
	}

	return true;
}
