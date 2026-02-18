/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 1:13:27 AM
 * =====================
 *
 */


#pragma once

#include <sg/CommonCore.h>

struct Contents;

NS_SG_BEGIN
extern ::Contents Contents;
NS_END

void InitializeClientCore();
void FinalizeClientCore();
