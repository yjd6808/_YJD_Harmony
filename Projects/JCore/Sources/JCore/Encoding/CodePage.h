/*
 * 작성자: 윤정도
 * 생성일: 12/5/2022 10:18:06 PM
 * =====================
 *
 * 인코딩 관련 학습: https://blog.naver.com/reversing_joa/222939705304
 * 마이크로소프트 코드 페이지 식별자: https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
 */


#pragma once

#include <JCore/Namespace.h>

NS_JC_BEGIN

enum CodePage
{
    ANSI = 0,         // 시스템 로컬 인코딩
    CP949 = 51949,
    UTF8 = 65001,
    UTF16LE = 1200,
    UTF16BE = 1201,
    UTF32LE = 12000,
    UTF32BE = 12001
};

NS_JC_END




