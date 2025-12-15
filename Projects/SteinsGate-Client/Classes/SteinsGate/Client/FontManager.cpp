/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 2:38:08 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "FontManager.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/JsonUtilEx.h>

#include <JCore/FileSystem/Directory.h>
#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>


USING_NS_JC;
USING_NS_JS;

//////////////////////////////////////////////////////////////////////////////////////////
FontManager::FontManager()
: fontNameToCodeMap_(8)
, fontCodeToNameMap_(8)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
FontManager::~FontManager()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void FontManager::init()
{
	SGString fontDirPath = JCore::Path::Combine(Core::CommonInfo->dataPath_, Const::Resource::FontDirName);
	Vector<SGString> fontFiles = Directory::Files(fontDirPath.Source());

	for (int index = 0; index < fontFiles.Size(); ++index)
	{
		SGString& fontPath = fontFiles[index];
		SGString fontFileName = JCore::Path::FileName(fontPath);

		fontNameToCodeMap_.Insert(fontFileName, index);
		fontCodeToNameMap_.Insert(index, Move(fontFileName)); // 버리기 아까우니 옴긴다.
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
SGString& FontManager::getFontName(int _fontCode)
{
	DebugAssertMsg(fontCodeToNameMap_.Exist(_fontCode), "해당 폰트코드에 맞는 폰트 이름이 없습니다.");
	return fontCodeToNameMap_[_fontCode];
}

//////////////////////////////////////////////////////////////////////////////////////////
int FontManager::getFontCode(const SGString& _fontName)
{
	DebugAssertMsg(fontNameToCodeMap_.Exist(_fontName), "해당 폰트이름에 맞는 폰트 코드가 없습니다.");
	return fontNameToCodeMap_[_fontName];
}
