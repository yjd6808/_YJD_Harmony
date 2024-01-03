/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#pragma once


#include <JCore/Pool/BinarySearchMemoryPool.h>

NS_PACKET_POOLING_BEGIN

enum class TestMode
{
	OnNetworking,			// 패킷 송/수신 이벤트 처리동안 풀링이 얼마나 영향 주는지 (네트워킹 시간 포함)
	OnSending				// 송신 오버랩이 완료까지만 측정했을때 얼마나 영향 주는지 (네트워킹 시간 포함 X)
};

extern int TestClientCount;
extern int TestSendCount;
extern bool UsePooling;
extern AutoResetEvent TestFinished;		// 테스트 완료여부
extern thread_local SharedPtr<BinarySearchMemoryPool> TlsMemPool;
extern TestMode Mode;

NS_PACKET_POOLING_END