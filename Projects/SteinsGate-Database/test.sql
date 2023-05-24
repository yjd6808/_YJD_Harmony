use steinsgate;

select * from t_account;


select c_account_id
	 , c_id
	 , c_pass
	 , CONVERT_TZ(c_created, '+00:00', @@global.time_zone) as c_tz
     , unix_timestamp(CONVERT_TZ(c_created, '+00:00', @@global.time_zone)) as c_create
     , unix_timestamp(c_created2) as c_create2
     , unix_timestamp(c_logined) as c_logined 
  from t_account

