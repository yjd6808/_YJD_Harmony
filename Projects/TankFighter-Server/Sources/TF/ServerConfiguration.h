/*
 * 작성자 : 윤정도
 */

#pragma once

#define DB_HOST "127.0.0.1"
#define DB_ID	"root"
#define DB_PASS	"1234"
#define DB_NAME	"tankfighter"
#define DB_PORT 3307
#define DB_CONN_POOL_SIZE			25							// DB 커넥션 풀 크기
#define TCP_MAXIMUM_CONNECTION		32							// 최대 연결 허용 세션 수
#define PLAYER_POOL_SIZE			TCP_MAXIMUM_CONNECTION		// 플레이어 풀 크기는 세션 풀 크기만큼 해주자.
#define ROOM_POOL_SIZE				10							// 각 채널마다 룸 풀의 크기는 10개로 잡자.

#define BATTLE_FIELD_WORKER_DELAY			100		// 채널별로 존재하는 배틀진행중인 방에 대해서 몇 밀리초마다 각 RoomState에 맞는 루틴을 진행할지
#define BATTLE_FIELD_PLAYWAIT_TIME			7000	// RoomState가 PlayWait 상태로 얼마나 둘지 - 즉 이 시간동안 다른 플레이어가 로딩할때까지 실제 게임을 진행하지 않고 기다려준다.
#define BATTLE_FIELD_PLAYING_TIME			10000	// 이 시간동안 게임을 진행한다.
#define BATTLE_FIELD_ENDWAIT_TIME			10000	// 이 시간동안 게임을 하면서 플레이어가 기록한 통계 수치를 보여준다. 이시간이 만료되면 배틀이 끝난것으로 보고 모든 플레이어를 방으로 이동시킨다.
#define BATTLE_REVIVAL_TIME					5000	// 죽으면 이 시간이 지난뒤에 부활한다.

#define UDP_BIND_ADDR	"0.0.0.0"			// UDP 안씀
#define UDP_BIND_PORT	9999				// UDP 안씀

#define TCP_BIND_ADDR	"0.0.0.0"
#define TCP_BIND_PORT	9999