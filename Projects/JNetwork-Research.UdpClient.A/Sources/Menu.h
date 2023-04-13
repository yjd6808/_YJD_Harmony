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
	eConnectClientB,
	eSendLogin,
	eSendChannel,
	eSendGame,
	eSendClientB,
	eBindRecvFrom,
	eDisconnect,
};

void ConnectLogin();
void ConnectChannel();
void ConnectGame();
void ConnectClientB();
void SendLogin();
void SendChannel();
void SendGame();
void SendClientB();
void BindRecvFrom();
void Disconnect();