/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:56:56 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGJson.h"

#include <SteinsGate/Client/SGConfig.h>
#include <SteinsGate/Client/SGAnimationInfo.h>
#include <SteinsGate/Client/SGTextParser.h>

SGString SGJson::getString(Json::Value& value) {
	DebugAssertMsg(!value.isNull(), "값이 없습니다.");
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

		SGString frameInfo = SGJson::getString(frameRoot);
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

void SGJson::parseThicknessInfo(Json::Value& thicknessRoot, SGThicknessBox& info) {

	int num[4];
	SGTextParser::parserIntNumbers(SGJson::getString(thicknessRoot), num, 4);

	info.RelativeX = num[0];
	info.RelativeY = num[1];
	info.Width = num[2];
	info.Height = num[3];
}

void SGJson::parseIntNumber3(Json::Value& root, int& num1, int& num2, int& num3) {
	int num[3];
	SGTextParser::parserIntNumbers(SGJson::getString(root), num, 3);

	num1 = num[0];
	num2 = num[1];
	num3 = num[2];
}

void SGJson::parseIntNumber4(Json::Value& root, int& num1, int& num2, int& num3, int& num4) {

	int num[4];
	SGTextParser::parserIntNumbers(SGJson::getString(root), num, 4);

	num1 = num[0];
	num2 = num[1];
	num3 = num[2];
	num4 = num[3];
}

void SGJson::parseIntNumberN(Json::Value& root, int* numArr, int count) {
	SGTextParser::parserIntNumbers(SGJson::getString(root), numArr, count);
}

void SGJson::parseFloatNumber2(Json::Value& root, float& num1, float& num2) {
	float num[2];
	SGTextParser::parserFloatNumbers(SGJson::getString(root), num, 2);
	num1 = num[0];
	num2 = num[1];
}

void SGJson::parseFloatNumberN(Json::Value& root, float* numArr, float count) {
	SGTextParser::parserFloatNumbers(SGJson::getString(root), numArr, count);
}

SGString SGJson::getStringDefault(Json::Value& value, const SGString& defaultValue) {
	if (value.empty())
		return defaultValue;

	return getString(value);
}

float SGJson::getFloatDefault(Json::Value& value, float defaultValue) {
	if (value.empty())
		return defaultValue;

	return value.asFloat();
}

int SGJson::getIntDefault(Json::Value& value, int defaultValue) {
	if (value.empty())
		return defaultValue;

	return value.asInt();
}