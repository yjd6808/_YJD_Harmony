/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:34:00 AM
 * =====================
 *
 */


#pragma once


class CommonInfo;
struct CharCommonInfo;
struct ServerProcessInfo;

extern CommonInfo* CoreCommonInfo_v;
extern CharCommonInfo* CoreCharCommon_v;
extern ServerProcessInfo* CoreServerProcessInfo_v;

void InitializeCommonCore();
void FinalizeCommonCore();