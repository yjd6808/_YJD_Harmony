/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 9:54:20 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/Pair.h>
#include <JCore/Primitives/String.h>

#include <SteinsGate/Common/SGEnumDefine.h>

SEnumBegin(CommonInputEvent)
NoEvent,
TerminateProgram,
CommonMax
SEnumMiddle(CommonInputEvent)
static const char* CommonName[CommonMax];
SEnumMiddleEnd(CommonInputEvent)

// =================================================

SEnumInteritedBegin(AuthInputEvent, CommonInputEvent)
AuthMax = 2
SEnumMiddle(AuthInputEvent)
static const char* AuthName[AuthMax];
static const char* NameOf(int inputEvent);
static JCore::Pair<JCore::String, int> PairOf(int inputEvent);
SEnumMiddleEnd(AuthInputEvent)

