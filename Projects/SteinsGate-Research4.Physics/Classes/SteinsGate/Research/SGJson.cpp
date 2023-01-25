/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:56:56 PM
 * =====================
 *
 */


#include "SGJson.h"

#include <SteinsGate/Research/Config.h>
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
	info.Code = animationRoot["code"].asInt();
	info.Loop = animationRoot["loop"].asBool();

	Json::Value& frameListRoot = animationRoot["frames"];
	for (int i = 0; i < frameListRoot.size(); ++i) {
		Json::Value& frameRoot = frameListRoot[i];

		int iFrameIndex;	// 필수
		int iDelay;			// 필수
		int iFrameEvent = InvalidValue_v;
		int iFrameEventId = InvalidValue_v;

		SGTextParser::parserFrameInfo(SGJson::getString(frameRoot), iFrameIndex, iDelay, iFrameEvent, iFrameEventId);
		info.Frames.EmplaceBack(iFrameIndex, (float)iDelay / 1000.0f, iFrameEvent, iFrameEventId);
	}
}

void SGJson::parseThicknessInfo(Json::Value& thicknessRoot, SGThicknessBox& info) {

	int num[4];
	SGTextParser::parserNumbers(SGJson::getString(thicknessRoot), num, 4);

	info.RelativeX = num[0];
	info.RelativeY = num[1];
	info.Width = num[2];
	info.Height = num[3];
}

void SGJson::parseNumber3(Json::Value& root, int& num1, int& num2, int& num3) {
	int num[3];
	SGTextParser::parserNumbers(SGJson::getString(root), num, 3);

	num1 = num[0];
	num2 = num[1];
	num3 = num[2];
}

void SGJson::parseNumber4(Json::Value& root, int& num1, int& num2, int& num3, int& num4) {
	int num[4];
	SGTextParser::parserNumbers(SGJson::getString(root), num, 4);

	num1 = num[0];
	num2 = num[1];
	num3 = num[2];
	num4 = num[3];
}

void SGJson::parseNumberN(Json::Value& root, int* numArr, int capacity) {
	SGTextParser::parserNumbers(SGJson::getString(root), numArr, capacity);
}

