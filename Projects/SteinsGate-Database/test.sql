use steinsgate;

# 기존 테이블에 컬럼 추가
alter table t_account add column c_created3 timestamp(3) not null after c_created2;

# 기존 테이블에 컬럼 여러개 추가
alter table t_account 
	add column c_created4 timestamp(3) not null after c_created3,
	add column gender enum('Male', 'Female') not null after c_created3;
						
# 테이블 컬럼 정보보기
show columns from t_account;

# 데이터 삽입
insert into t_account (c_id, c_pass) values ('wjdeh515', 'wjdeh414');

# 데이터 삭제
delete from t_account;
