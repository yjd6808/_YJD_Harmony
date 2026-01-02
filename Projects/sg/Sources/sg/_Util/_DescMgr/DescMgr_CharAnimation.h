#pragma once

#include <sg/_Util/DescLoaderAbstract.h>

struct CharAnimationInfoLoader : DescLoaderAbstract
{
public:
	CharAnimationInfoLoader(DescLoaderMgr* _pManager);
	~CharAnimationInfoLoader() override = default;

	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::Char_Animation; }
	bool Load() override;

	jc::Vector<AnimationInfo*>& GetAnimationList(CharType_t _charCode);

private:
	jc::Vector<AnimationInfo*> charAnimationList_[CharType::Max];
};
