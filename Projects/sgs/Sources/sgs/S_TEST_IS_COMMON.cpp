/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 4:01:24 PM
 * =====================
 *
 */

#include "S_TEST_IS_COMMON.h"

#include "jc/Primitives/StringConvert.h"
#include "sgs/CmdRelay_TEST.h"


USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
bool S_TEST_IS_COMMON::SEND_SS_P2PRelayStaticTest(const char* _pMsg)
{
	auto sending = SendBegin<SS_P2PRelayStaticTest>();
	sending.cmd_.Msg.SetString(StringConvert::FromUtf8(_pMsg));
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool S_TEST_IS_COMMON::SEND_SS_P2PRelayDynamicTest(const char* _pMsg)
{
	auto sending = SendBegin<SS_P2PRelayDynamicTest>(64);
	const String wideMsg = StringConvert::FromUtf8(_pMsg);
	sending.cmd_.Msg()->SetStringUnsafe(wideMsg.Source());
	return true;
}
