/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 1:13:27 AM
 * =====================
 *
 */


#pragma once

#include <sg/CommonCore.h>

class SteinsGateApp;
class NetCore;
struct Contents;

NS_SG_BEGIN
extern ::SteinsGateApp* App;
extern ::Contents Contents;
extern ::NetCore* Net;
NS_SG_END

void InitializeClientCore();
void FinalizeClientCore();
