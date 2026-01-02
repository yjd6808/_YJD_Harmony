/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 2:38:00 AM
 * =====================
 *
 */


#pragma once

class FontManager final : public jc::SingletonPointer<FontManager>
{
	friend class TSingleton;
	FontManager();
	~FontManager();

public:
	void Init();
	jc::String& GetFontName(int _fontCode);
	jc::String  GetFontPath(int _fontCode);
	int GetFontCode(const jc::String& _fontName);
	int GetFontCount() const { return fontCodeToNameMap_.Size(); }

private:
	jc::HashMap<jc::String, int> fontNameToCodeMap_;
	jc::HashMap<int, jc::String> fontCodeToNameMap_;
};

#define g_cFontMgr (*FontManager::Get())
