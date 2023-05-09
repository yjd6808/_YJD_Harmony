/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:37:07 PM
 * =====================
 *
 * IOCPThreadAccessPacketQueue는 IOCP 쓰레드에서 수신한 패킷을 복사한 패킷을 담기위한 용도의 큐이다.
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "NetClientEventListener.h"

#include <SteinsGate/Client/CommandSynchronizer.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;


void NetClientEventListener::SynchronizedOnReceived(Session* session, ICommand* cmd) {
	CommandSynchronizer::Get()->enqueueCommand(session, cmd);
}

