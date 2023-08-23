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

void JsonUtilEx::parseAnimationInfo(Json::Value& animationRoot, AnimationInfo& info) {
	info.Name = getStringOrNull(animationRoot["name"]);
	info.Code = getIntDefault(animationRoot["code"], 1); 
	info.Loop = getBooleanDefault(animationRoot["loop"]);
	if (!info.Name.IsNull() && info.Name == "sliding")
		int a = 40;
	Json::Value& frameListRoot = animationRoot["frames"];
	for (int i = 0; i < frameListRoot.size(); ++i) {
		Json::Value& frameRoot = frameListRoot[i];

		int iFrameIndex;	// 필수
		int iDelay;			// 필수
		int iFrameEventId = InvalidValue_v;

		int szFrameLen;
		const char* szFrame = getStringRaw(frameRoot, &szFrameLen);
		TextParser::parseFrameInfo(szFrame, szFrameLen, iFrameIndex, iDelay, iFrameEventId);
		info.Frames.EmplaceBack(iFrameIndex, (float)iDelay / 1000.0f, iFrameEventId);
	}
}

void JsonUtilEx::parseActorRect(Json::Value& root, ActorRect& actorRect) {
	int actorRectData[8];
	parseIntNumberN(root, actorRectData, sizeof(actorRectData) / sizeof(int));

	actorRect.ThicknessRect.origin.x = actorRectData[0];
	actorRect.ThicknessRect.origin.y = actorRectData[1];
	actorRect.ThicknessRect.size.width = actorRectData[2];
	actorRect.ThicknessRect.size.height = actorRectData[3];

	actorRect.BodyRect.origin.x = actorRectData[4];
	actorRect.BodyRect.origin.y = actorRectData[5];
	actorRect.BodyRect.size.width = actorRectData[6];
	actorRect.BodyRect.size.height = actorRectData[7];
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

void JsonUtilEx::parseActorSpriteData(Json::Value& actorSpriteDataRoot, JCORE_OUT ActorSpriteData** info) {
	Json::Value& partListRoot = actorSpriteDataRoot["parts"];
	Json::Value& animationListRoot = actorSpriteDataRoot["animation"];
	int iPrevPartZOrder = 0;


	const ActorPartSpritePositioningRule_t ePositioningRule = (ActorPartSpritePositioningRule_t)getIntDefault(actorSpriteDataRoot["positioning_rule"], 0);;
	const int iPartCount = partListRoot.size();
	const int iAnimationCount = animationListRoot.size();
	
	ActorSpriteData* pInfo = dbg_new ActorSpriteData{ePositioningRule, iPartCount, iAnimationCount };
	pInfo->PositioningRule = ePositioningRule;

	for (int i = 0; i < iPartCount; i++) {
		Json::Value& partRoot = partListRoot[i];

		SGString sgaName = getString(partRoot["sga"]);
		SGString imgName = getString(partRoot["img"]);
		ImagePack* pImgPack = Core::Contents.PackManager->getPack(sgaName);

		const int iZOrder = getIntDefault(partRoot["z_order"], iPrevPartZOrder + 1);	// z_order가 없는 경우 이전 파츠 인덱스보다 점점 더 커지도록 
		const int iSga = pImgPack->getPackIndex();
		const int iImg = pImgPack->getImgIndex(imgName);
		
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

		pInfo->Parts.EmplaceBack(iZOrder, iSga, iImg);
		iPrevPartZOrder = iZOrder;
	}

	DebugAssert(pInfo->Parts.Capacity() == iPartCount);		// parts 값 로딩 후 벡터 용량이 변경된 경우 (이런 경우는 없겠지?)
	DebugAssert(iAnimationCount > 0);						// 액터에 애니메이션이 하나도 없는 경우

	for (int i = 0; i < iAnimationCount; ++i) {
		Json::Value& animationRoot = animationListRoot[i];
		const int iFrameCount = animationRoot["frames"].size();
		AnimationInfo animation { iFrameCount };
		parseAnimationInfo(animationListRoot[i], animation);
		pInfo->Animations.PushBack(Move(animation));
	}

	DebugAssert(pInfo->Animations.Capacity() == iAnimationCount); // animation 값 로딩 후 벡터 용량이 변경된 경우 (이런 경우는 없겠지?)

	*info = pInfo;
}

void JsonUtilEx::parseColor4B(Json::Value& root, JCORE_OUT SGColor4B& color) {

	int iR;
	int iG;
	int iB;
	int iA;

	parseIntNumber4(root, iR, iG, iB, iA);
	DebugAssertMsg(iR >= 0 && iR <= MaxByte_v, "R 색상값(%d)이 0 ~ %d사이 값이 아닙니다.", iR, MaxByte_v);
	DebugAssertMsg(iG >= 0 && iG <= MaxByte_v, "G 색상값(%d)이 0 ~ %d사이 값이 아닙니다.", iG, MaxByte_v);
	DebugAssertMsg(iB >= 0 && iB <= MaxByte_v, "B 색상값(%d)이 0 ~ %d사이 값이 아닙니다.", iB, MaxByte_v);
	DebugAssertMsg(iA >= 0 && iA <= MaxByte_v, "A 색상값(%d)이 0 ~ %d사이 값이 아닙니다.", iA, MaxByte_v);

	color.r = (GLubyte)iR;
	color.g = (GLubyte)iG;
	color.b = (GLubyte)iB;
	color.a = (GLubyte)iA;
}

void JsonUtilEx::parseSize(Json::Value& root, SGSize& size) {
	parseFloatNumber2(root, size.width, size.height);
}

void JsonUtilEx::parseVec2(Json::Value& root, SGVec2& vec) {
	parseFloatNumber2(root, vec.x, vec.y);
}
