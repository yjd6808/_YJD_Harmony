/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/CommonCore.h>


struct ServerInfo;

extern ServerInfo* CoreServer_v;

void InitializeServerCore();
void FinalizeServerCore();