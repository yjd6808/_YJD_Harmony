/*
 * 작성자: 윤정도
 * 생성일: 6/17/2023 8:36:35 PM
 * =====================
 *
 */


#include "InterServerRecvHelper.h"

#include "sg/Const/Const.h"


thread_local _s8 InterServerRecvHelper::LastFromId = InvalidValue_v;
