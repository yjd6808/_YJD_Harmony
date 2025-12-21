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
	~FontManager();

public:
	void Init();
	SGString& GetFontName(int _fontCode);
	int GetFontCode(const SGString& _fontName);
	int GetFontCount() const { return fontCodeToNameMap_.Size(); }

private:
	SGHashMap<SGString, int> fontNameToCodeMap_;
	SGHashMap<int, SGString> fontCodeToNameMap_;
};
