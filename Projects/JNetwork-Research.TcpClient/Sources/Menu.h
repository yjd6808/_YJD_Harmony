/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 10:32:50 PM
 * =====================
 *
 */


#pragma once

enum Menu
{
	eConnectLogin = 1,
	eConnectChannel,
	eConnectGame,
	eSendLogin,
	eSendChannel,
	eSendGame,
	eDisconnectLogin,
	eDisconnectChannel,
	eDisconnectGame
};

void ConnectLogin();
void ConnectChannel();
void ConnectGame();
void SendLogin();
void SendChannel();
void SendGame();
void DisconnectLogin();
void DisconnectChannel();
void DisconnectGame();