/*
 * �ۼ��� : ������
 */

#pragma once

#define DB_HOST "127.0.0.1"
#define DB_ID	"root"
#define DB_PASS	"1234"
#define DB_NAME	"tankfighter"
#define DB_PORT 3307
#define DB_CONN_POOL_SIZE			25							// DB Ŀ�ؼ� Ǯ ũ��
#define TCP_MAXIMUM_CONNECTION		32							// �ִ� ���� ��� ���� ��
#define PLAYER_POOL_SIZE			TCP_MAXIMUM_CONNECTION		// �÷��̾� Ǯ ũ��� ���� Ǯ ũ�⸸ŭ ������.
#define ROOM_POOL_SIZE				10							// �� ä�θ��� �� Ǯ�� ũ��� 10���� ����.

#define BATTLE_FIELD_WORKER_DELAY			100		// ä�κ��� �����ϴ� ��Ʋ�������� �濡 ���ؼ� �� �и��ʸ��� �� RoomState�� �´� ��ƾ�� ��������
#define BATTLE_FIELD_PLAYWAIT_TIME			7000	// RoomState�� PlayWait ���·� �󸶳� ���� - �� �� �ð����� �ٸ� �÷��̾ �ε��Ҷ����� ���� ������ �������� �ʰ� ��ٷ��ش�.
#define BATTLE_FIELD_PLAYING_TIME			10000	// �� �ð����� ������ �����Ѵ�.
#define BATTLE_FIELD_ENDWAIT_TIME			10000	// �� �ð����� ������ �ϸ鼭 �÷��̾ ����� ��� ��ġ�� �����ش�. �̽ð��� ����Ǹ� ��Ʋ�� ���������� ���� ��� �÷��̾ ������ �̵���Ų��.
#define BATTLE_REVIVAL_TIME					5000	// ������ �� �ð��� �����ڿ� ��Ȱ�Ѵ�.

#define UDP_BIND_ADDR	"0.0.0.0"			// UDP �Ⱦ�
#define UDP_BIND_PORT	9999				// UDP �Ⱦ�

#define TCP_BIND_ADDR	"0.0.0.0"
#define TCP_BIND_PORT	9999