/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#include "PacketPooling_Common.h"

NS_PACKET_POOLING_BEGIN
int TestClientCount;
int TestSendCount;
bool UsePooling;
NormalLock TestLock;
ConditionVariable TestCondVar;
int TestStep;
TestMode Mode;
thread_local SharedPtr<CBinarySearchMemoryPool> TlsMemPool;
NS_PACKET_POOLING_END
