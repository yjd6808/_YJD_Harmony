/*
 * 작성자: 윤정도
 * 생성일: 10/9/2023 4:29:06 PM
 * =====================
 *
 */


#include "Pch.h"


void InitializeGameCore() {
	Core::NetCore = dbg_new NetCore;
	Core::NetCore->Initialize();

	Core::GameClient = Core::NetCore->getGameClient();
	Core::Room = dbg_new Room;
}

void FinalizeGameCore() {
	JCORE_DELETE_SAFE(Core::Room);
	JCORE_DELETE_SAFE(Core::NetCore);

}
