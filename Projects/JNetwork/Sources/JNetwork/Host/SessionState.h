/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 11:42:19 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Namespace.h>

NS_JNET_BEGIN

enum SessionState
{
	eInitailized = 1,
	eAcceptWait = 2,		// 서버에서 연결 수락 대기
	eConnectWait = 2,		// 클라에서 연결 시도 대기
	eAccepted = 3,			// 서버에서 연결 수략 완료
	eConnected = 3,			// 클라에서 연결 완료
	eDisconnected = 4
};

NS_JNET_END