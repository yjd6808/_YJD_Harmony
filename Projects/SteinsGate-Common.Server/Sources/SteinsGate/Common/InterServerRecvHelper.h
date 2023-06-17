/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 8:19:07 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Type.h>

struct InterServerRecvHelper
{
	static thread_local Int8 LastFromId;						// 누구에게서 받았는지	
};


