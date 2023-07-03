/*
 * 작성자: 윤정도
 * 생성일: 6/17/2023 8:36:35 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "InterServerRecvHelper.h"

#include <SteinsGate/Common/Const.h>


thread_local Int8 InterServerRecvHelper::LastFromId = InvalidValue_v;

