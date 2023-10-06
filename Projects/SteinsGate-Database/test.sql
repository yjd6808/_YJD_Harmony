use steinsgate;

select * from t_account;



# 기존 테이블에 컬럼 추가
alter table t_account add column c_created3 timestamp(3) not null after c_created2;

# 기존 테이블에 컬럼 여러개 추가
alter table t_account 
	add column c_created4 timestamp(3) not null after c_created3,
	add column gender enum('Male', 'Female') not null after c_created3;
    
# 기존 테이블의 컬럼 제거
alter table t_account 
	drop column c_created2,
	drop column c_created3,
    drop column c_created4,
    drop column gender;
						
# 테이블 컬럼 정보보기
show columns from t_account;

# 데이터 삽입
insert into t_account (c_id, c_pass) values ('wjdeh515', 'wjdeh414');

# 데이터 삭제
delete from t_account;

alter table t_account add column c_last_server tinyint default 0 not null;

drop table tmp_table;
create temporary table tmp_table (
	user_id varchar(20),
    amount int
);

insert into tmp_table (user_id, amount) values 
('a', 5000),
('a', 10000),
('b', 5000),
('b', 10000),
('b', 15000),
('b', 20000),
('a', 15000);

select gg.user_id, count(*) from (select user_id, amount from tmp_table where amount >= 10000) as gg group by gg.user_id;
