/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 9:04:30 PM
 * =====================
 *
 */


#pragma once

#include "sg/PCH/Core.h"
#include "jdb/PCH/Core.h"

#include "sgs/ServerConst.h"
#include "sgs/ServerEnum.h"

#include "mysql.h"
#include "conio.h"

enum net_host_class : _u8
{
	nhServer		= 0x3,
	nhGameServer	= 0x4,
	nhClient		= 0x6,
	nhClientCustom  = 0x7,		// 값 지정용도
};


// 게임서버 핸들 구조
// | 넷 그룹ID(8) | 세션 클래스(8) | 게임 서버 타입(8) - 루크, 힐더... | 채널 번호(8) | 안씀(16) |

// 서버 핸들 구조
// | 넷 그룹ID(8) | 세션 클래스(8) | 서버 타입(8) | 안씀(8) | 안씀(16) |

// 세션 핸들 구조 -> 서버 핸들 정보를 담고 있음.
// |  상위 4바이트 서버 핸들 정보  | 세션 인덱스(16) |


#define NET_GID_MAIN		0
#define NET_GID_INTER_SERV	1

// 서버 핸들: session_id 없이 상위 필드만 채움
#define make_net_game_server_handle(group_id, game_server_type, channel_num) \
	( ((_u64)(group_id)			<< 56)		| \
	  ((_u64)(nhGameServer)		<< 48)		| \
	  ((_u64)(game_server_type)	<< 40)		| \
	  ((_u64)(channel_num)		<< 32) )

#define make_net_server_handle(group_id, server_type) \
	( ((_u64)(group_id)			<< 56)		| \
	  ((_u64)(nhServer)			<< 48)		| \
	  ((_u64)(server_type)		<< 40) )

#define make_net_custom_handle(group_id, host_class, index) \
	( ((_u64)(group_id)			<< 56)		| \
	  ((_u64)(host_class)		<< 48)		| \
	  ((_u64)(index)			& 0xffffULL) )

// 핸들에서 각 필드 추출
#define get_group_id_from_sh(sh)         ((_u8)(((sh) >> 56) & 0xFF))
#define get_host_class_from_sh(sh)		 ((_u8)(((sh) >> 48) & 0xFF))
#define get_server_type_from_sh(sh)      ((_u8)(((sh) >> 40) & 0xFF))
#define get_channel_number_from_sh(sh)   ((_u8)(((sh) >> 32) & 0xFF))
#define get_session_idx_from_sh(sh)      ((_u16)((sh) & 0xffffULL))

// 고정 세션 핸들 ID
#define SH_INTER_SERV_TCP		make_net_custom_handle(NET_GID_INTER_SERV, nhClientCustom, 0)
#define SH_INTER_SERV_UDP		make_net_custom_handle(NET_GID_INTER_SERV, nhClientCustom, 1)

// 루크 서버 대역		1'000
// 루크 서버 1채널	1'001
// 루크 서버 2채널	1'002
#define NET_GAME_SERVER_UNIT			1'000
#define NET_SESSION_HANDLE_UNIT		  100'000
