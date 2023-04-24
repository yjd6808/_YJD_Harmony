/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 2:38:00 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"

class FontPackage
{
public:
	FontPackage();

	void init();

	static FontPackage* get() {
		static FontPackage* inst = nullptr;

		if (inst == nullptr) {
			inst = new FontPackage;
		}

		return inst;
	}

	SGString& getFontName(int fontCode);
	int getFontCode(const SGString& fontName);

private:
	SGHashMap<SGString, int> m_hFontNameToCodeMap;
	SGHashMap<int, SGString> m_hFontCodeToNameMap;
};


