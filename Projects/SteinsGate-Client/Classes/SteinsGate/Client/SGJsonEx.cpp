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

USING_NS_JC;
USING_NS_CC;

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

void SGJsonEx::parseColor4B(Json::Value& root, Out_ SGColor4B& color) {

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

void SGJsonEx::parseSize(Json::Value& root, SGSize& size) {
	parseFloatNumber2(root, size.width, size.height);
}

void SGJsonEx::parseVec2(Json::Value& root, SGVec2& vec) {
	parseFloatNumber2(root, vec.x, vec.y);
}
