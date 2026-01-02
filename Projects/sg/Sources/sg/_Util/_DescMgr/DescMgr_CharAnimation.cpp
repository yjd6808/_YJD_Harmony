#include "Core.h"
#include "DescMgr_CharAnimation.h"

#include <sg/_Util/JsonUtil.h>

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
CharAnimationInfoLoader::CharAnimationInfoLoader()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CharAnimationInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
		return false;

	try
	{
		for (int charCode = 0; charCode < CharType::Max; ++charCode)
		{
			Value& animationListRoot = root[CharType::Name[charCode]];
			for (size_t i = 0; i < animationListRoot.size(); ++i)
			{
				Value& animationRoot = animationListRoot[(ArrayIndex)i];
				AnimationInfo* pInfo = dbg_new AnimationInfo(animationRoot["frames"].size());
				JsonUtil::ParseAnimationInfo(animationRoot, *pInfo);
				charAnimationList_[charCode].PushBack(pInfo);
				AddData(pInfo);
			}
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 0c9f 18s  05dchi 13b3 15c6  16d1d 0c  01b 14c6 1477 ", GetConfigFileName(), ex.what());
		return false;
	}

	Loaded();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::Vector<AnimationInfo*>& CharAnimationInfoLoader::GetAnimationList(CharType_t _charCode)
{
	jc_assert_msg(_charCode >= CharType::Begin && _charCode <= CharType::End, " 03c6salt  1d71 19ff 08b5 02f6 1520 ");
	return charAnimationList_[_charCode];
}
