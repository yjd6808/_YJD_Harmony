# < TaskFighter 프로젝트 SQL 스크립트 >
# 작성자 : 윤정도
# 
# mysql server version : 5.7.37
# mysql workbench 8.0.27
# encoding : utf8
# encdoing collation : utf8_unicode_520_ci

drop schema if exists tankfighter ;											# 만약 스키마가 존재한다면 제거하도록 한다.
create schema tankfighter character set utf8 collate utf8_unicode_520_ci;	# 스키마를 utf8 인코딩으로 생성한다.
use tankfighter;
set sql_mode='allow_invalid_dates';											# timestamp 디폴트 값 설정가능하도록 https://stackoverflow.com/questions/9192027/invalid-default-value-for-create-date-timestamp-field

create table t_channel (
	c_uid				int				primary key auto_increment		comment '채널 고유 아이디',
    c_name				varchar(60)		not null						comment '채널 명',
    c_max_player_count	int				not null						comment '최대로 접속가능한 플레이어 수',
	c_time				timestamp(6)	not null 						comment	'추가된 시간'
) character set utf8 collate utf8_unicode_520_ci;

insert into t_channel (c_name, c_max_player_count, c_time) values ('초보 정도 채널', 2, now(6));
insert into t_channel (c_name, c_max_player_count, c_time) values ('중수 정도 채널', 3, now(6));
insert into t_channel (c_name, c_max_player_count, c_time) values ('고수 정도 채널', 4, now(6));
insert into t_channel (c_name, c_max_player_count, c_time) values ('초고수 정도 채널', 5, now(6));

# 비밀번호 저장할 때 MD5() 함수로 저장할 것
# 32
create table t_account (
	c_uid					int 			primary key auto_increment		comment	'계정 고유 아이디',
    c_id					varchar(40)		not null						comment '계정 아이디',
    c_pass					char(32)		not	null						comment	'계정 MD5 비밀번호',
    c_last_logout_time		timestamp(6)									comment '마지막으로 로그아웃한 시간',
    c_last_login_time		timestamp(6)									comment '마지막으로 로그인한 시간'
) character set utf8 collate utf8_unicode_520_ci;

# 임시 계정 넣기
-- insert into t_account values (1, 'wjdeh515', md5('wjdeh414'), now(6), now(6));
-- insert into t_account values (2, 'wjdeh616', md5('wjdeh515'), now(6), now(6));
-- insert into t_account values (3, 'wjdeh717', md5('wjdeh616'), now(6), now(6));
-- insert into t_account values (4, 'wjdeh818', md5('wjdeh717'), now(6), now(6));
-- insert into t_account values (5, 'wjdeh919', md5('wjdeh818'), now(6), now(6));
-- insert into t_account values (6, 'wjdeh10110', md5('wjdeh919'), now(6), now(6));
-- insert into t_account values (7, 'wjdeh11111', md5('wjdeh10110'), now(6), now(6));

# 캐릭터 테이블
# 계정이 삭제되면 관련 캐릭터들도 모두 삭제 처리 되도록 on delete cascade 사용
create table t_character (
	c_uid					int				primary key auto_increment		comment '캐릭터 고유 아이디',
    c_account_uid			int				not null 						comment	'해당 캐릭터를 가진 계정 고유 아이디',
    c_channel_uid			int				not	null						comment	'해당 캐릭터가 속한 채널',
    c_name					varchar(60)		not null						comment '캐릭터 이름',
	c_win					int				not null default 0				comment '캐릭터 승리 횟수',
    c_lose					int				not null default 0				comment '캐릭터 패배 횟수',
    c_kill					int				not null default 0				comment '캐릭터의 킬 수',
    c_death					int				not null default 0				comment '캐릭터의 데스 수',
    c_money					int				not null default 0				comment	'캐릭터가 소지중인 금액',
 
	constraint t_character_fk_account_uid 	foreign key (c_account_uid) references t_account(c_uid) on delete cascade,
    constraint t_character_fk_channed_uid 	foreign key (c_channel_uid) references t_channel(c_uid) on delete cascade
) character set utf8 collate utf8_unicode_520_ci;





# 친구 관계
create table t_friendship (
	c_req_character_uid		int				not null				comment	'친구를 요청한 캐릭터의 고유 ID',
    c_ack_character_uid		int				not null				comment '친구를 수락한 캐릭터의 고유 ID',
    
    constraint t_character_fk_req_character_uid 	foreign key (c_req_character_uid) references t_character(c_uid) on delete cascade, 
    constraint t_character_fk_ack_character_uid 	foreign key (c_ack_character_uid) references t_character(c_uid) on delete cascade
) character set utf8 collate utf8_unicode_520_ci;

# 인벤토리
create table t_inventory (
	c_uid					int				primary key auto_increment	comment	'인벤토리 고유 아이디',
    c_character_uid			int				not null					comment '이 인벤토리를 소요한 캐릭터의 고유 ID',
    c_size					int				not null					comment '인벤토리 크기',

	constraint t_inventory_fk_character_uid foreign key (c_character_uid) references t_character(c_uid) on delete cascade
) character set utf8 collate utf8_unicode_520_ci;

# 소지중인 아이템 정보
# 아이템 고유 아이디는 설정 파일에서 값이 정의됨
create table t_inventoryitem (
	c_inventory_uid			int				not null					comment '이 아이템이 속한 인벤토리의 고유 ID',
    c_character_uid			int				not null					comment '이 아이템을 소지한 캐릭터의 ID',
    c_item_uid				int				not null					comment	'아이템 고유 ID',
    c_quantity				int				not null					comment	'소지중인 아이템 수',
    
	constraint t_inventoryitem_fk_inventory_uid foreign key (c_inventory_uid) references t_inventory(c_uid) on delete cascade,
    constraint t_inventoryitem_fk_character_uid foreign key (c_character_uid) references t_character(c_uid) on delete cascade   
) character set utf8 collate utf8_unicode_520_ci


/* 참고사항 1
Mysql 디폴트 인코딩 변경 방법 :
C:\ProgramData\MySQL\MySQL Server 5.7 경로 들어가서 my.ini에 아래 옵션들 추가 해준 후 서버 껏다 키면됨

[client]
default-character-set=utf8

[mysql]
default-character-set=utf8


[mysqld]
collation-server = utf8_unicode_520_ci
init-connect='SET NAMES utf8'
character-set-server = utf8
*/