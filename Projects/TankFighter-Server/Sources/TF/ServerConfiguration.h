#pragma once

#define DB_HOST "127.0.0.1"
#define DB_ID	"root"
#define DB_PASS	"1234"
#define DB_NAME	"tankfighter"
#define DB_PORT 3307
#define DB_CONN_POOL_SIZE			25		// DB Ŀ�ؼ� Ǯ ũ��
#define TCP_MAXIMUM_CONNECTION		32		// �ִ� ���� ��� ���� ��
#define PLAYER_POOL_SIZE			TCP_MAXIMUM_CONNECTION		// �÷��̾� Ǯ ũ��� ���� Ǯ ũ�⸸ŭ ������.
#define ROOM_POOL_SIZE				10							// �� ä�θ��� �� Ǯ�� ũ��� 10���� ����.


#define UDP_BIND_ADDR	"0.0.0.0"
#define UDP_BIND_PORT	9999

#define TCP_BIND_ADDR	"0.0.0.0"
#define TCP_BIND_PORT	9999