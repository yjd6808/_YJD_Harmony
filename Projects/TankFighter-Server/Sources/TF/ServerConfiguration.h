#pragma once

#define DB_HOST "127.0.0.1"
#define DB_ID	"root"
#define DB_PASS	"1234"
#define DB_NAME	"tankfighter"
#define DB_PORT 3307
#define DB_CONN_POOL_SIZE			25		// DB 커넥션 풀 크기
#define TCP_MAXIMUM_CONNECTION		32		// 최대 연결 허용 세션 수
#define PLAYER_POOL_SIZE			TCP_MAXIMUM_CONNECTION		// 플레이어 풀 크기는 세션 풀 크기만큼 해주자.
#define ROOM_POOL_SIZE				10							// 각 채널마다 룸 풀의 크기는 10개로 잡자.


#define UDP_BIND_ADDR	"0.0.0.0"
#define UDP_BIND_PORT	9999

#define TCP_BIND_ADDR	"0.0.0.0"
#define TCP_BIND_PORT	9999