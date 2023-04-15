/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 3:14:36 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerEnum.h"

USING_NS_JC;

const char* CommonInputEvent::CommonName[CommonMax] {
	"__no_name__",
	"exit"
};


const char* AuthInputEvent::AuthName[AuthMax]{
	"__no_name__",
	"__no_name__"
};

const char* AuthInputEvent::NameOf(int inputEvent) {
	if (inputEvent < CommonMax) {
		return CommonName[inputEvent];
	}

	return AuthName[inputEvent];
}

Pair<String, int> AuthInputEvent::PairOf(int inputEvent) {
	return { NameOf(inputEvent), inputEvent };
}
