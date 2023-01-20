/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:56:56 PM
 * =====================
 *
 */


#include "SGJson.h"

#include <SteinsGate/Research/SGAnimationInfo.h>
#include <SteinsGate/Research/SGTextParser.h>

SGString SGJson::getString(Json::Value& value) {
	DebugAssertMessage(!value.isNull(), "값이 없습니다.");
	const char* pBegin;
	const char* pEnd;
	value.getString(&pBegin, &pEnd);
	const int iLen = pEnd - pBegin;
	return { pBegin, iLen + 1 };
}


void SGJson::parseAnimationInfo(Json::Value& animationRoot, SGAnimationInfo& info) {
	info.Name = SGJson::getString(animationRoot["name"]);
	info.Loop = animationRoot["loop"].asBool();

	Json::Value& frameListRoot = animationRoot["frames"];
	for (int i = 0; i < frameListRoot.size(); ++i) {
		Json::Value& frameRoot = frameListRoot[i];

		int iFrameIndex;
		int iDelay;
		int iFrameEvent;
		int iFrameEventId;

		SGTextParser::parserFrameInfo(SGJson::getString(frameRoot), iFrameIndex, iDelay, iFrameEvent, iFrameEventId);
		info.Frames.EmplaceBack(iFrameIndex, (float)iDelay / 1000.0f, iFrameEvent, iFrameEventId);
	}
}