select * from  t_login 
where usertype=1 and logtime>="20071211000000" and logtime<"20071212000000"
select count(*) userlogin,  count(DISTINCT userid),  from  t_login where usertype=1 and logtime>="20071211000000" and logtime<"20071212000000";
insert into t_user_info 
(select "20071211000000",1 , count(*) userlogin,  count(DISTINCT userid) uinlogin, sum(onlinetime),0,0,0,0,0,0  from  t_login  where usertype=1 and logtime>="20071211000000" and logtime<"20071212000000")
union
(select count(*)  from  t_login  where usertype=1 and logtime>="20071211000000" and logtime<"20071212000000" and onlinetime<10)
