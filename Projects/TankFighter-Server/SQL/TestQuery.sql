insert into t_account (c_id, c_pass) values ('wjdeh515', md5('wjdeh414'));

select * from t_channel;
select * from t_account;
select * from t_character;
select * from t_friendship;

update t_channel set c_max_player_count = 5 where c_name = '초고수 정도 채널';
select count(*) from t_character where c_name = 'fse';
delete from t_channel where c_name like '%정도%';

delete from t_friendship where 
	(c_req_character_uid = 1 and c_ack_character_uid = 2) or (c_ack_character_uid = 2 and c_req_character_uid = 1);
    
    
select now(6);