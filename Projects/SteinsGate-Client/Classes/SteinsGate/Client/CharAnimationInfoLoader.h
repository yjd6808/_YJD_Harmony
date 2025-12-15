#pragma once

#include <SteinsGate/Client/Tutturu.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct CharAnimationInfoLoader : ConfigFileLoaderAbstract
{
public:
	CharAnimationInfoLoader(DataManagerAbstract* _pManager);
	~CharAnimationInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::Char_Animation; }
	bool Load() override;

	SGVector<AnimationInfo*>& GetAnimationList(CharType_t _charCode);

private:
	SGVector<AnimationInfo*> charAnimationList_[CharType::Max];
};
