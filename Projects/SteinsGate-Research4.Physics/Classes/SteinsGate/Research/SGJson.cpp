/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:56:56 PM
 * =====================
 *
 */


#include "SGJson.h"

SGString SGJson::getString(Json::Value& value) {
	DebugAssertMessage(!value.isNull(), "값이 없습니다.");
	const char* pBegin;
	const char* pEnd;
	value.getString(&pBegin, &pEnd);
	const int iLen = pEnd - pBegin;
	return { pBegin, iLen + 1 };
}