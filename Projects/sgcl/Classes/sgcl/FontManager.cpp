/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 2:38:08 AM
 * =====================
 *
 */


#include "Core.h"
#include "FontManager.h"
#include "GameCoreHeader.h"

#include <sg/Config.h>

#include <jc/FileSystem/Directory.h>
#include <jc/FileSystem/Path.h>

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
void FontManager::Init()
{
	const jc::String& fontDir = CONF_GET_STRING(CONF_PROP_RES_DATA_FONT_PATH);
	Vector<jc::String> fontFiles = Directory::Files(fontDir);

	for (int index = 0; index < fontFiles.Size(); ++index)
	{
		jc::String& fontPath = fontFiles[index];
		jc::String fontFileName = jc::Path::FileName(fontPath);

		fontNameToCodeMap_.Insert(fontFileName, index);
		fontCodeToNameMap_.Insert(index, Move(fontFileName)); // 버리기 아까우니 옴긴다.
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String& FontManager::GetFontName(int _fontCode)
{
	jc_assert_msg(fontCodeToNameMap_.Exist(_fontCode), "해당 폰트코드에 맞는 폰트 이름이 없습니다.");
	return fontCodeToNameMap_[_fontCode];
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String FontManager::GetFontPath(int _fontCode)
{
	const jc::String& fontName = GetFontName(_fontCode);
	const jc::String fontPath = jc::Path::Combine(CONF_GET_STRING(CONF_PROP_RES_DATA_FONT_PATH), fontName);
	return fontPath;
}

//////////////////////////////////////////////////////////////////////////////////////////
int FontManager::GetFontCode(const jc::String& _fontName)
{
	jc_assert_msg(fontNameToCodeMap_.Exist(_fontName), "해당 폰트이름에 맞는 폰트 코드가 없습니다.");
	return fontNameToCodeMap_[_fontName];
}
