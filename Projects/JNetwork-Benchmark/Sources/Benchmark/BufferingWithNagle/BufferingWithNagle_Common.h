/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#pragma once


NS_BUFFERING_WITH_NAGLE_BEGIN

extern int TestClientCount;
extern int TestSendCount;
extern bool AsyncSending;
extern AutoResetEvent TestFinished;		// 테스트 완료여부

NS_BUFFERING_WITH_NAGLE_END