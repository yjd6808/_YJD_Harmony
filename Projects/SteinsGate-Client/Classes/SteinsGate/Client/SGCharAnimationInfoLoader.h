/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct SGCharAnimationInfoLoader : ConfigFileLoaderAbstract
{
public:
	~SGCharAnimationInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Char_Animation; }
	bool load() override;

	SGVector<SGAnimationInfo*>& getAnimationList(CharType_t charCode);
private:
	SGVector<SGAnimationInfo*> m_CharAnimationList[CharType::Max];
};