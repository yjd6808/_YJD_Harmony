/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 2:38:00 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"

class SGFontPackage
{
public:
	SGFontPackage();

	void init();

	static SGFontPackage* get() {
		static SGFontPackage* inst = nullptr;

		if (inst == nullptr) {
			inst = new SGFontPackage;
		}

		return inst;
	}

	SGString& getFontName(int fontCode);
	int getFontCode(const SGString& fontName);

private:
	SGHashMap<SGString, int> m_hFontNameToCodeMap;
	SGHashMap<int, SGString> m_hFontCodeToNameMap;
};


