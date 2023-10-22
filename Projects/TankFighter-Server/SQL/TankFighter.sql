# < TaskFighter 프로젝트 SQL 스크립트 >
# 작성자 : 윤정도
# 
# mysql server version : 5.7.37
# mysql workbench 8.0.27
# encoding : utf8
# encdoing collation : utf8_unicode_520_ci


/* 

참고사항 1
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

insert into t_channel (c_name, c_max_player_count, c_time) values ('초보 정도 채널', 1000, now(6));
insert into t_channel (c_name, c_max_player_count, c_time) values ('중수 정도 채널', 1000, now(6));
insert into t_channel (c_name, c_max_player_count, c_time) values ('고수 정도 채널', 1000, now(6));
insert into t_channel (c_name, c_max_player_count, c_time) values ('초고수 정도 채널', 1000, now(6));

# 비밀번호 저장할 때 MD5() 함수로 저장할 것
# 32
create table t_account (
	c_uid					int 			primary key auto_increment		comment	'계정 고유 아이디',
    c_id					varchar(40)		not null unique					comment '계정 아이디',
    c_pass					char(32)		not	null						comment	'계정 MD5 비밀번호',
    c_last_logout_time		timestamp(6)									comment '마지막으로 로그아웃한 시간',
    c_last_login_time		timestamp(6)									comment '마지막으로 로그인한 시간'
) character set utf8 collate utf8_unicode_520_ci;

# 캐릭터 테이블
# 계정이 삭제되면 관련 캐릭터들도 모두 삭제 처리 되도록 on delete cascade 사용
# [인덱스]
# 이름으로 검색을 자주할 수 있으니 이름은 인덱스를 걸음
# [유니크]
# 채널당 별로 이름은 유일해야하므로 채널 ID와 이름을 unique로 묶음
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
	index t_character_ky_name(c_name), 
	constraint t_character_fk_account_uid 	foreign key (c_account_uid) references t_account(c_uid) on delete cascade,
    constraint t_character_fk_channed_uid 	foreign key (c_channel_uid) references t_channel(c_uid) on delete cascade,
    constraint t_character_uc_name          unique (c_channel_uid, c_name)
) character set utf8 collate utf8_unicode_520_ci;





# 친구 관계
# [인덱스]
# 친구 요청자를 빠르게 찾기위해 req 인덱스처리
# 친구 수락자를 빠르게 찾기위해 ack 인덱스처리
# [유니크]
# 중복 친구 방지를 위해 req, ack uid를 묶어서 unique 처리함
create table t_friendship (
	c_req_character_uid		int				not null				comment	'친구를 요청한 캐릭터의 고유 ID',
    c_ack_character_uid		int				not null				comment '친구를 수락한 캐릭터의 고유 ID',
    
    index t_friendship_ky_req_uid(c_req_character_uid),
    index t_friendship_ky_ack_uid(c_ack_character_uid),
    constraint t_friendship_fk_req_character_uid 	foreign key (c_req_character_uid) references t_character(c_uid) on delete cascade, 
    constraint t_friendship_fk_ack_character_uid 	foreign key (c_ack_character_uid) references t_character(c_uid) on delete cascade,
    constraint t_friendship_uc_req_ack_uid			unique(c_req_character_uid, c_ack_character_uid)
) character set utf8 collate utf8_unicode_520_ci;



# 테스트용 계정 미리 넣어놓기
drop procedure if exists insert_many_account;
delimiter $$
create procedure insert_many_account(
    in count int
)
begin    
    declare _i int default 0;
    prepare stmt from 'insert into t_account (c_id, c_pass) values (?, md5(?))';
    
    delete from t_friendship;
    delete from t_character;
    delete from t_account;
    
    while (_i <= count) do
        set @id = concat('wjdeh', _i);
        set @pass = concat('wjdeh', _i);
        execute stmt using @id, @pass;
        set _i = _i + 1;
    end while;
    
    deallocate prepare stmt;
end$$
delimiter ;






# 각 계정별로 채널당 캐릭터를 count만큼 생성합니다.
# 캐릭터명은 계정명이 wjdeh1이라면 wjdeh1-0, wjdeh1-1 이렇게 순서대로 캐릭터명 생성토록함
drop procedure if exists insert_many_character;
delimiter $$
create procedure insert_many_character(
	in count int
)
begin    
	declare _i int default 0;
    declare _account_uid int;
    declare _account_id varchar(30);
    declare _channel_uid int;
    declare _is_cursor_end bool default false;
    declare _cur_channel_uid cursor for select c_uid from t_channel;
    declare _cur_account_id cursor for select c_uid, c_id from t_account;
    declare continue handler for not found set _is_cursor_end = true;

    prepare stmt from 'insert into t_character (c_account_uid, c_channel_uid, c_name) values (?, ?, ?)';
    
    delete from t_friendship;
    delete from t_character;

    open _cur_account_id;    
    
L1:	loop
		fetch _cur_account_id into _account_uid, _account_id;
		if (_is_cursor_end = true) then
			close _cur_account_id;
			leave L1;
		end if;
		
		set @account_uid = _account_uid;
		set @account_id = _account_id;
    
		open _cur_channel_uid;
L2:		loop
			
			fetch _cur_channel_uid into _channel_uid;
            if (_is_cursor_end = true) then
				close _cur_channel_uid;
				set _is_cursor_end = false;
				leave L2;
			end if;
            set @channel_uid = _channel_uid;
            set _i = 0;
            while (_i < count) do
				set @character_name = concat(@account_id, '-', _i);
				execute stmt using 	@account_uid, @channel_uid, @character_name;
                set _i = _i + 1;
            end while;
		end loop L2;
    end loop L1;
	select _debug_log;
    
    deallocate prepare stmt;
end$$
delimiter ;

call insert_many_account(100);
call insert_many_character(10);