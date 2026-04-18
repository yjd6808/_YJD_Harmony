/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:56:56 PM
 * =====================
 *
 */

#include "Util/JsonUtilEx.h"

#include "sgcl/Game/Texture/ImagePackManager.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtilEx::ParseActorSpriteData(Json::Value& _actorSpriteDataRoot, OUT ActorSpriteData** _ppInfo)
{
	Json::Value& partListRoot = _actorSpriteDataRoot["parts"];
	Json::Value& animationListRoot = _actorSpriteDataRoot["animation"];
	int prevPartZOrder = 0;

	const ActorPartSpritePositioningRule_t positioningRule = (ActorPartSpritePositioningRule_t)GetIntDefault(
		_actorSpriteDataRoot["positioning_rule"], 0);
	const int partCount = partListRoot.size();
	const int animationCount = animationListRoot.size();

	ActorSpriteData* pInfo = dbg_new ActorSpriteData{ positioningRule, partCount, animationCount };
	pInfo->positioningRule_ = positioningRule;

	for (int i = 0; i < partCount; i++)
	{
		Json::Value& partRoot = partListRoot[i];

		jc::String sgaName = GetString(partRoot["sga"]);
		jc::String imgName = GetString(partRoot["img"]);
		ImagePack* pImgPack = g_cImagePackMgr.GetPack(sgaName);

		const int zOrder = GetIntDefault(partRoot["z_order"], prevPartZOrder + 1);
		// z_order가 없는 경우 이전 파츠 인덱스보다 점점 더 커지도록
		const int sgaIndex = pImgPack->GetPackIndex();
		const int imgIndex = pImgPack->GetImgIndex(imgName);

		//ActorPartSpriteData* pPartSpriteData = nullptr;

		//switch (ePositioningRule) {
		//case ActorPartSpritePositioningRule::InFrameSize:
		//	pPartSpriteData = dbg_new ActorPartSpriteData{ iZOrder, iSga, iImg };
		//	break;
		//case ActorPartSpritePositioningRule::InCustomFrameSize:
		//	ActorPartSpriteDataCustom* pPartSpriteDataCustom = dbg_new ActorPartSpriteDataCustom{ iZOrder, iSga, iImg };
		//	// pPartSpriteDataCustom->CustomSizeInfo.X =
		//	pPartSpriteData = pPartSpriteDataCustom;
		//	break;
		//case ActorPartSpritePositioningRule::InIgnoredFrameSize:
		//	pPartSpriteData = dbg_new ActorPartSpriteData{ iZOrder, iSga, iImg };
		//	break;
		//}
		// jc_assert(pPartSpriteData != nullptr);

		pInfo->parts_.EmplaceBack(zOrder, sgaIndex, imgIndex);
		prevPartZOrder = zOrder;
	}

	jc_assert(pInfo->parts_.Capacity() == partCount); // parts 값 로딩 후 벡터 용량이 변경된 경우 (이런 경우는 없겠지?)
	jc_assert(animationCount > 0); // 액터에 애니메이션이 하나도 없는 경우

	for (int i = 0; i < animationCount; ++i)
	{
		Json::Value& animationRoot = animationListRoot[i];
		const int frameCount = animationRoot["frames"].size();
		AnimationInfo animation{ frameCount };
		ParseAnimationInfo(animationListRoot[i], animation);
		pInfo->animations_.PushBack(Move(animation));
	}

	jc_assert(pInfo->animations_.Capacity() == animationCount); // animation 값 로딩 후 벡터 용량이 변경된 경우 (이런 경우는 없겠지?)

	*_ppInfo = pInfo;
}


USING_NS_JC;

