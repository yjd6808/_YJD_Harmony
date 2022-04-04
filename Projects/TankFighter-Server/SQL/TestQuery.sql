insert into t_account (c_id, c_pass) values ('wjdeh515', md5('wjdeh414'));

select * from t_channel;
select * from t_account;

update t_channel set c_max_player_count = 5 where c_name = '초고수 정도 채널';
select count(*) from t_character where c_name = 'fse';



select now(6);