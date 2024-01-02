/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#include <JCore/Time.h>
#include "Common.h"

volatile bool TestFinished_v = false;
bool DontEcho_v;
JCore::StopWatch<JCore::StopWatchMode::HighResolution> StopWatch_v;
JCore::TimeSpan LastTime_v;