use steinsgate;

select * from t_account;


select c_account_id
	 , c_id
	 , c_pass
     , c_created
     , c_created2t_account
	 , CONVERT_TZ(c_created, '+00:00', @@global.time_zone) as c_zz1
     , unix_timestamp(CONVERT_TZ(c_created, '+00:00', @@global.time_zone)) as c_zz1
     , unix_timestamp(c_created2) as c_zz2
     , unix_timestamp(c_logined) as c_logined 
  from t_account
  
  
select * from t_account where c_id = 'wjdeh717' and c_pass = 'wjdeh717';
  
# delete from t_account where 

