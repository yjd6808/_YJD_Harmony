/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:34:00 AM
 * =====================
 *
 */


#pragma once


class CommonInfo;
struct SGCharCommonInfo;

extern CommonInfo* CoreCommonInfo_v;
extern SGCharCommonInfo* CoreCharCommon_v;

void InitializeCommonCore();
void FinalizeCommonCore();