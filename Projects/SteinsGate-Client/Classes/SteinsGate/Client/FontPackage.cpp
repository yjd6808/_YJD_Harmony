/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 2:38:08 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "FontPackage.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/JsonUtilEx.h>

#include <JCore/FileSystem/Directory.h>
#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>




USING_NS_JC;
USING_NS_JS;

FontPackage::FontPackage()
	: m_hFontNameToCodeMap(8)
	, m_hFontCodeToNameMap(8)
{}

FontPackage::~FontPackage() {
}

void FontPackage::init() {
	SGString szFontDirPath = JCore::Path::Combine(CoreCommonInfo_v->DataPath, Const::Resource::FontDirName);
	Vector<SGString> vFontFiles = Directory::Files(szFontDirPath.Source());

	for (int i = 0; i < vFontFiles.Size(); ++i) {
		SGString& fontPath = vFontFiles[i];
		SGString fontFileName = JCore::Path::FileName(fontPath);

		m_hFontNameToCodeMap.Insert(fontFileName, i);
		m_hFontCodeToNameMap.Insert(i, Move(fontFileName));	// 버리기 아까우니 옴긴다.
	}

}

SGString& FontPackage::getFontName(int fontCode) {
	DebugAssertMsg(m_hFontCodeToNameMap.Exist(fontCode), "해당 폰트코드에 맞는 폰트 이름이 없습니다.");
	return m_hFontCodeToNameMap[fontCode];
}

int FontPackage::getFontCode(const SGString& fontName) {
	DebugAssertMsg(m_hFontNameToCodeMap.Exist(fontName), "해당 폰트이름에 맞는 폰트 코드가 없습니다.");
	return m_hFontNameToCodeMap[fontName];
}


