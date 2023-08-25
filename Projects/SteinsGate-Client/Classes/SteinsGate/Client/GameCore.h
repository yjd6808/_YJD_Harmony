/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 1:13:27 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonCore.h>

class SteinsGateApp;
class ClientInfo;
class DataManager;
class NetCore;

struct RuntimeConfig;
struct Contents;

NS_CORE_BEGIN
extern ::SteinsGateApp* App;
extern ::ClientInfo* ClientInfo;
extern ::Contents Contents;
extern ::DataManager* DataManager;
extern ::NetCore* Net;
extern ::RuntimeConfig* RuntimeConfig;
NS_CORE_END

void InitializeClientCore();
void FinalizeClientCore();