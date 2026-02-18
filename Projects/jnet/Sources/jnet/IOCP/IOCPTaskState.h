/*
 * 작성자: 윤정도
 * 생성일: 2/15/2026 6:21:23 PM
 * =====================
 *
 */


#pragma once

struct IOCPTaskState
{
	enum _
	{
		eInitialized,
		eRunning,
		eReady,
		eFinished
	};
};