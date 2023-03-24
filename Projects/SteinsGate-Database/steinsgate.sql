# < SteinsGate 프로젝트 SQL 스크립트 >
# 작성자 : 윤정도
# 
# mysql server version : 8.0.31
# mysql workbench 8.0.31
# encoding : utf8
# encdoing collation : utf8_unicode_520_ci

drop database if exists steinsgate;
create schema steinsgate character set utf8 collate utf8_unicode_520_ci;
use steinsgate;
set sql_mode='allow_invalid_dates';

create table t_account (
	c_uid	int			primary key
)