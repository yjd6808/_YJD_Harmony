# < SteinsGate 프로젝트 SQL 스크립트 >
# 작성자 : 윤정도
# 
# mysql server version : 8.0.31
# mysql workbench 8.0.31

drop schema if exists steinsgate;
create schema steinsgate character set utf8mb4 collate utf8mb4_unicode_ci;
use steinsgate;
set sql_mode='allow_invalid_dates';

-- 계정 테이블
drop table if exists t_account;
create table t_account (
	c_uid				int				primary key auto_increment			comment '계정 PK',
    c_account_id		varchar(20) 	unique								comment '계정 아이디',
    c_account_pass		varchar(20)		not null							comment '계정 비번',
    c_created			timestamp		default current_timestamp			comment '계정 생성날짜',
    c_logined			timestamp		default 0							comment '계정 접속날짜',
    c_last_login_server tinyint         default 0                           comment '마지막 접속한 게임서버 ID'
);


-- 캐릭터 테이블
-- 계정:캐릭터=One:Many
drop table if exists t_character;
create table t_character (
	c_uid				int				primary key auto_increment			comment '캐릭터 PK',
    c_account_uid		int				not null							comment '소유 계정 FK',
    c_name				varchar(20)		unique								comment '캐릭터 닉넴',
    c_type				smallint		not null							comment '캐릭터 타입',
    c_town				smallint		not null							comment '마을',
    c_created			timestamp		default current_timestamp			comment '캐릭터 생성날짜',
    c_logined			timestamp		null								comment '캐릭터 접속날짜',
    c_level				smallint		default 1							comment '캐릭터 레벨',
    c_str				int				default 0							comment '캐릭터 힘',
    c_dex				int				default 0							comment '캐릭터 민첩성',
    c_vit				int				default 0							comment '캐릭터 생명력',
    c_int				int				default 0							comment '캐릭터 지능',
    c_hp				int 			default 0							comment '캐릭터 HP',
    c_mp				int 			default 0							comment '캐릭터 MP',
    c_left_stat			int				default 0							comment '부여가능한 남은 스탯',
    
	constraint t_character_fk_account_id foreign key (c_account_uid) references t_account(c_uid) on delete cascade
);


-- 아이템 어트리뷰트
-- 장비아이템에 부여될 옵션과 수치
-- 아이템:속성=One:One
drop table if exists t_attribute;
create table t_attribute (
	c_uid				int				primary key auto_increment			comment '아이템 속성 PK',
    c_base_opt_1		tinyint			default 0							comment '기본 옵션 1',
    c_base_value_1		smallint		default 0							comment '기본 옵션 1 수치',
    c_base_opt_2		tinyint			default 0							comment '기본 옵션 2',
    c_base_value_2		smallint		default 0							comment '기본 옵션 2 수치',
    c_opt_1				tinyint			default 0							comment '부가 옵션 1',
    c_value_1			smallint		default 0							comment '부가 옵션 1 수치',
    c_opt_2				tinyint			default 0							comment '부가 옵션 2',
    c_value_2			smallint		default 0							comment '부가 옵션 2 수치',
    c_opt_3				tinyint			default 0							comment '부가 옵션 3',
    c_value_3			smallint		default 0							comment '부가 옵션 3 수치',
    c_opt_4				tinyint			default 0							comment '부가 옵션 4',
    c_value_4			smallint		default 0							comment '부가 옵션 4 수치',
    c_opt_5				tinyint			default 0							comment '부가 옵션 5',
    c_value_5			smallint		default 0							comment '부가 옵션 5 수치',
    c_opt_6				tinyint			default 0							comment '부가 옵션 6',
    c_value_6			smallint		default 0							comment '부가 옵션 6 수치'
);


-- 아이템 테이블
-- 캐릭터:아이템=One:Many
-- c_item_code는 아래와같이 복합적인 구조로 구성했다.
-- 31     26      22       15       0
-- +-------+-------+-------+--------+
-- |  Type |Detail2|Detail1|  Code  |
-- +-------+-------+-------+--------+
-- Detail1: 아바타 부위 || 갑옷 종류 || 무기 종류
-- Detail2: 착용가능한 캐릭터 타입
-- Type: 소모품, 아바타, 무기등 타입을 의미
-- 자세한 구조는 SGStruct.h 파일 참고
drop table if exists t_item;
create table t_item (
	c_uid					int				primary key auto_increment			comment '아이템 PK',
	c_character_id			int				not null							comment '아이템 소유 캐릭터 FK',
    c_attribute_id			int				default null						comment '아이템 능력치 FK(장비류)',
    c_item_code				int				not null							comment '아이템 코드',
    c_slot					smallint		not null							comment '아이템 슬롯 인덱스',
    c_stored				tinyint			not null							comment '아이템 저장인벤(장비, 소모품, 기타, 퀘스트, 아바타, 창고)',
    c_count					smallint		not null							comment	'아이템 갯수',
    
    constraint t_item_fk_character_id foreign key (c_character_id) references t_character(c_uid) on delete cascade,
    constraint t_item_fk_attribute_id foreign key (c_attribute_id) references t_attribute(c_uid) on delete cascade
);



-- [초대 테이블]
-- 아마 구현하게되면 친구 초대만 구현할꺼지만.. 만약, 길드초대, 원정대 초대 같은 기능을 구현한다면 가정해서 테이블 구조를 구성했다.
-- 안쓸수도 있을 듯
-- SGEnum.h의 InvitationType
drop table if exists t_invitation;
create table t_invitation (
	c_uid						int				primary key auto_increment			comment '초대 PK',
    c_type						tinyint			not null							comment '초대 종류',
    c_requested					timestamp		default current_timestamp			comment '초대한 날짜',
    c_host_character_uid		int				not null							comment '초대요청자 FK',
    c_guest_character_uid		int				not null							comment '초대수신자 FK',
    
    -- 요청보낸 사람의 캐릭터가 삭제되면 초대도 없어지도록 하는게 맞겠지?
    constraint t_invitation_fk_host_character_id foreign key (c_host_character_uid) references t_character(c_uid) on delete cascade
);

-- [친구 관계 테이블] t_invitationt_friendship
-- 수락, 요청자 합쳐서 PK
drop table if exists t_friendship;
create table t_friendship (
    c_req_character_uid			int			  	not null							comment '초대요청자 FK',
    c_ack_character_uid			int			  	not null							comment '초대수락자 FK',
    c_matched					timestamp		default current_timestamp			comment '친구맺은날',
    
    constraint t_friendship_pk primary key (c_req_character_uid, c_ack_character_uid),
    constraint t_friendship_fk_req_character_id foreign key (c_req_character_uid) references t_character(c_uid) on delete cascade,
    constraint t_friendship_fk_ack_character_id foreign key (c_ack_character_uid) references t_character(c_uid) on delete cascade
);

