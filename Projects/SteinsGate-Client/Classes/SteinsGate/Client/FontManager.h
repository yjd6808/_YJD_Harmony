/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 2:38:00 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

class FontManager final : public JCore::SingletonPointer<FontManager>
{
	friend class TSingleton;
	FontManager();
	~FontManager() override;
public:
	void init();
	SGString& getFontName(int fontCode);
	int getFontCode(const SGString& fontName);
	int getFontCount() const { return m_hFontCodeToNameMap.Size(); }

private:
	SGHashMap<SGString, int> m_hFontNameToCodeMap;
	SGHashMap<int, SGString> m_hFontCodeToNameMap;
};


