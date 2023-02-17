/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:56:56 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGJsonEx.h"

#include <SteinsGate/Client/SGAnimationInfo.h>
#include <SteinsGate/Common/SGTextParser.h>

void SGJsonEx::parseAnimationInfo(Json::Value& animationRoot, SGAnimationInfo& info) {
	info.Name = SGJsonEx::getString(animationRoot["name"]);
	info.Code = animationRoot["code"].asInt();
	info.Loop = animationRoot["loop"].asBool();

	Json::Value& frameListRoot = animationRoot["frames"];
	for (int i = 0; i < frameListRoot.size(); ++i) {
		Json::Value& frameRoot = frameListRoot[i];

		int iFrameIndex;	// 필수
		int iDelay;			// 필수
		int iFrameEvent = InvalidValue_v;
		int iFrameEventId = InvalidValue_v;

		SGString frameInfo = SGJsonEx::getString(frameRoot);
		char* pInstantAttackBoxSourceOffset = SGTextParser::parserFrameInfo(frameInfo, iFrameIndex, iDelay, iFrameEvent, iFrameEventId);

		// 인스턴트 공격 박스외에는 전부 4개의 값만 넣어주면 됨
		if (iFrameEvent == FrameEventType::AttackBoxInstant) {
			float instantAttackBoxData[8];

			SGTextParser::parserFloatNumbers(
				pInstantAttackBoxSourceOffset, 
				SGStringUtil::Length(pInstantAttackBoxSourceOffset), 
				instantAttackBoxData, 
				8
			);

			SGFrameInfoAttackBoxInstant* pInfo = dbg_new SGFrameInfoAttackBoxInstant{};
			pInfo->FrameIndex = iFrameIndex;
			pInfo->Delay = (float)iDelay / 1000.0f;
			pInfo->FrameEvent = iFrameEvent;
			pInfo->FrameEventId = iFrameEventId;

			pInfo->Rect.ThicknessRect.origin.x = instantAttackBoxData[0];
			pInfo->Rect.ThicknessRect.origin.y = instantAttackBoxData[1];
			pInfo->Rect.ThicknessRect.size.width = instantAttackBoxData[2];
			pInfo->Rect.ThicknessRect.size.height = instantAttackBoxData[3];

			pInfo->Rect.BodyRect.origin.x = instantAttackBoxData[4];
			pInfo->Rect.BodyRect.origin.y = instantAttackBoxData[5];
			pInfo->Rect.BodyRect.size.width = instantAttackBoxData[6];
			pInfo->Rect.BodyRect.size.height = instantAttackBoxData[7];

			info.Frames.PushBack(pInfo);
		} else {
			SGFrameInfo* pInfo = dbg_new SGFrameInfo{ iFrameIndex, (float)iDelay / 1000.0f, iFrameEvent, iFrameEventId };
			info.Frames.PushBack(pInfo);
		}
		
	}
}
