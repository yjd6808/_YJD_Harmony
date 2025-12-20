/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:56:56 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "JsonUtilEx.h"

#include <SteinsGate/Common/Struct.h>
#include <SteinsGate/Common/TextParser.h>

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtilEx::ParseAnimationInfo(Json::Value& _animationRoot, AnimationInfo& _info)
{
	_info.Name = GetStringOrNull(_animationRoot["name"]);
	_info.code_ = GetIntDefault(_animationRoot["code"], 1);
	_info.Loop = GetBooleanDefault(_animationRoot["loop"]);

	if (!_info.Name.IsNull() && _info.Name == "sliding")
	{
		int a = 40;
	}

	Json::Value& frameListRoot = _animationRoot["frames"];
	for (size_t i = 0; i < frameListRoot.size(); ++i)
	{
		Json::Value& frameRoot = frameListRoot[(Json::ArrayIndex)i];

		int frameIndex; // 필수
		int delay; // 필수
		int frameEventId = InvalidValue_v;

		int frameLength;
		const char* pFrame = GetStringRaw(frameRoot, &frameLength);
		TextParser::ParseFrameInfo(pFrame, frameLength, frameIndex, delay, frameEventId);
		_info.Frames.EmplaceBack(frameIndex, (float)delay / 1000.0f, frameEventId);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtilEx::ParseActorRect(Json::Value& _root, ActorRect& _actorRect)
{
	int actorRectData[8];
	ParseIntNumberN(_root, actorRectData, sizeof(actorRectData) / sizeof(int));

	_actorRect.ThicknessRect.origin.x = actorRectData[0];
	_actorRect.ThicknessRect.origin.y = actorRectData[1];
	_actorRect.ThicknessRect.size.width = actorRectData[2];
	_actorRect.ThicknessRect.size.height = actorRectData[3];

	_actorRect.BodyRect.origin.x = actorRectData[4];
	_actorRect.BodyRect.origin.y = actorRectData[5];
	_actorRect.BodyRect.size.width = actorRectData[6];
	_actorRect.BodyRect.size.height = actorRectData[7];
}

/**
 * TODO: ActorSpriteData 기능 개선 필요 (추후 좀더 확장성있게 개발할려면 필수적으로 수행되어야함.)
 * 현재 ActorSpriteData는 캐릭터, 몬스터의 애니메이션 재생 구현에 초점을 두고 기능을 구현해서.
 * 모든 파츠마다 동일한 애니메이션을 실행한다고 가정하여 구현하였기 때문에
 * 액터의 각 파츠가 다른 애니메이션으로 구성된 경우에 대해서 처리하지 못한다.
 *
 * runAnimation을 ActorSprite에서 처리하도록 하고 있는데.. 이걸 빼야하나.. ㅠㅠ
 * 우선 다 만들고 고민하는걸로..
 */

void JsonUtilEx::ParseActorSpriteData(Json::Value& _actorSpriteDataRoot, JCORE_OUT ActorSpriteData** _ppInfo)
{
	Json::Value& partListRoot = _actorSpriteDataRoot["parts"];
	Json::Value& animationListRoot = _actorSpriteDataRoot["animation"];
	int prevPartZOrder = 0;

	const ActorPartSpritePositioningRule_t positioningRule = (ActorPartSpritePositioningRule_t)GetIntDefault(
		_actorSpriteDataRoot["positioning_rule"], 0);
	const int partCount = partListRoot.size();
	const int animationCount = animationListRoot.size();

	ActorSpriteData* pInfo = dbg_new ActorSpriteData{ positioningRule, partCount, animationCount };
	pInfo->PositioningRule = positioningRule;

	for (int i = 0; i < partCount; i++)
	{
		Json::Value& partRoot = partListRoot[i];

		SGString sgaName = GetString(partRoot["sga"]);
		SGString imgName = GetString(partRoot["img"]);
		ImagePack* pImgPack = Core::Contents.PackManager->getPack(sgaName);

		const int zOrder = GetIntDefault(partRoot["z_order"], prevPartZOrder + 1);
		// z_order가 없는 경우 이전 파츠 인덱스보다 점점 더 커지도록
		const int sgaIndex = pImgPack->getPackIndex();
		const int imgIndex = pImgPack->getImgIndex(imgName);

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
		// DebugAssert(pPartSpriteData != nullptr);

		pInfo->Parts.EmplaceBack(zOrder, sgaIndex, imgIndex);
		prevPartZOrder = zOrder;
	}

	DebugAssert(pInfo->Parts.Capacity() == partCount); // parts 값 로딩 후 벡터 용량이 변경된 경우 (이런 경우는 없겠지?)
	DebugAssert(animationCount > 0); // 액터에 애니메이션이 하나도 없는 경우

	for (int i = 0; i < animationCount; ++i)
	{
		Json::Value& animationRoot = animationListRoot[i];
		const int frameCount = animationRoot["frames"].size();
		AnimationInfo animation{ frameCount };
		ParseAnimationInfo(animationListRoot[i], animation);
		pInfo->Animations.PushBack(Move(animation));
	}

	DebugAssert(pInfo->Animations.Capacity() == animationCount); // animation 값 로딩 후 벡터 용량이 변경된 경우 (이런 경우는 없겠지?)

	*_ppInfo = pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtilEx::ParseColor4B(Json::Value& _root, JCORE_OUT SGColor4B& _color)
{
	int r;
	int g;
	int b;
	int a;

	ParseIntNumber4(_root, r, g, b, a);
	DebugAssertMsg(r >= 0 && r <= MaxByte_v, "R 색상값(%d)이 0 ~ %d사이 값이 아닙니다.", r, MaxByte_v);
	DebugAssertMsg(g >= 0 && g <= MaxByte_v, "G 색상값(%d)이 0 ~ %d사이 값이 아닙니다.", g, MaxByte_v);
	DebugAssertMsg(b >= 0 && b <= MaxByte_v, "B 색상값(%d)이 0 ~ %d사이 값이 아닙니다.", b, MaxByte_v);
	DebugAssertMsg(a >= 0 && a <= MaxByte_v, "A 색상값(%d)이 0 ~ %d사이 값이 아닙니다.", a, MaxByte_v);

	_color.r = (GLubyte)r;
	_color.g = (GLubyte)g;
	_color.b = (GLubyte)b;
	_color.a = (GLubyte)a;
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtilEx::ParseSize(Json::Value& _root, SGSize& _size)
{
	ParseFloatNumber2(_root, _size.width, _size.height);
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtilEx::ParseVec2(Json::Value& _root, SGVec2& _vec)
{
	ParseFloatNumber2(_root, _vec.x, _vec.y);
}
