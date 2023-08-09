/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 1:44:44 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct CharAnimationInfoLoader : ConfigFileLoaderAbstract
{
public:
	CharAnimationInfoLoader(DataManagerAbstract* manager);
	~CharAnimationInfoLoader() override = default;

	ConfigFileType_t getConfigFileType() override { return ConfigFileType::Char_Animation; }
	bool load() override;

	SGVector<AnimationInfo*>& getAnimationList(CharType_t charCode);
private:
	SGVector<AnimationInfo*> m_CharAnimationList[CharType::Max];
};