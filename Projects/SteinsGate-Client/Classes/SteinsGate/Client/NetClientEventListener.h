/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

class NetClientEventListener : public JNetwork::ClientEventListener
{
protected:
	void SynchronizedOnReceived(SGSession* session, JNetwork::ICommand* cmd);
};


