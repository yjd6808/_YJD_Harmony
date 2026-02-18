/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 9:04:30 PM
 * =====================
 *
 */


#pragma once

#include <sg/Core.h>

#include <sgs/ServerConst.h>
#include <sgs/ServerEnum.h>

#include <mysql.h>
#include <conio.h>

#define NET_GID_MAIN		0
#define NET_GID_INTER_SERV	1

// 루크 서버 대역		1'000
// 루크 서버 1채널	1'001
// 루크 서버 2채널	1'002
#define NET_GAME_SERVER_UNIT			1'000
#define NET_SESSION_HANDLE_UNIT		  100'000
