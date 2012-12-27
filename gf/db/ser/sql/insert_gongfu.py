insert_1276="delete from GF_%02d.t_gongfu_7 where userid = 1276%02d;\
insert into GF_%02d.t_gongfu_7 values(1276%02d,1,1234213411,'mark%02d',1,1,1234514566,1,10000,1,1234,1234614566,4321);";
insert_1032="delete from GF_%02d.t_gongfu_3 where userid = 1032%02d;\
insert into GF_%02d.t_gongfu_3 values(1032%02d,1,1234213411,'canno%02d',1,1,1234514566,1,10000,1,1234,1234614566,4321);";
insert_1290="delete from GF_%02d.t_gongfu_9 where userid = 1290%02d;\
insert into GF_%02d.t_gongfu_9 values(1290%02d,1,1234213411,'dike%02d',1,1,1234514566,1,10000,1,1234,1234614566,4321);";
insert_1315="delete from GF_%02d.t_gongfu_1 where userid = 1315%02d;\
insert into GF_%02d.t_gongfu_1 values(1315%02d,1,1234213411,'evahan%02d',1,1,1234514566,1,10000,1,1234,1234614566,4321);";
insert_1272="delete from GF_%02d.t_gongfu_7 where userid = 1272%02d;\
insert into GF_%02d.t_gongfu_7 values(1272%02d,1,1234213411,'jameszhang%02d',1,1,1234514566,1,10000,1,1234,1234614566,4321);";
insert_1278="delete from GF_%02d.t_gongfu_7 where userid = 1278%02d;\
insert into GF_%02d.t_gongfu_7 values(1278%02d,1,1234213411,'joe%02d',1,1,1234514566,1,10000,1,1234,1234614566,4321);";
insert_1281="delete from GF_%02d.t_gongfu_8 where userid = 1281%02d;\
insert into GF_%02d.t_gongfu_8 values(1281%02d,1,1234213411,'seven%02d',1,1,1234514566,1,10000,1,1234,1234614566,4321);";
insert_1291="delete from GF_%02d.t_gongfu_9 where userid = 1291%02d;\
insert into GF_%02d.t_gongfu_9 values(1291%02d,1,1234213411,'shadow%02d',1,1,1234514566,1,10000,1,1234,1234614566,4321);";
insert_1024="delete from GF_%02d.t_gongfu_2 where userid = 1024%02d;\
insert into GF_%02d.t_gongfu_2 values(1024%02d,1,1234213411,'fred%02d',1,1,1234514566,1,10000,1,1234,1234614566,4321);";
insert_1266="delete from GF_%02d.t_gongfu_6 where userid = 1266%02d;\
insert into GF_%02d.t_gongfu_6 values(1266%02d,1,1234213411,'kevinchu%02d',1,1,1234514566,1,10000,1,1234,1234614566,4321);";
insert_1021="delete from GF_%02d.t_gongfu_2 where userid = 1021%02d;\
insert into GF_%02d.t_gongfu_2 values(1021%02d,1,1234213411,'LL%02d',1,1,1234514566,1,10000,1,1234,1234614566,4321);";
insert_1126="delete from GF_%02d.t_gongfu_2 where userid = 1126%02d;\
insert into GF_%02d.t_gongfu_2 values(1126%02d,1,1234213411,'tb%02d',1,1,1234514566,1,10000,1,1234,1234614566,4321);";
insert_1028="delete from GF_%02d.t_gongfu_2 where userid = 1028%02d;\
insert into GF_%02d.t_gongfu_2 values(1028%02d,1,1234213411,'andy%02d',1,1,1234514566,1,10000,1,1234,1234614566,4321);";
for i in range(100):
	print insert_1276 %(i,i,i,i,i)
	print insert_1032 %(i,i,i,i,i)
	print insert_1290 %(i,i,i,i,i)
	print insert_1315 %(i,i,i,i,i)
	print insert_1272 %(i,i,i,i,i)
	print insert_1278 %(i,i,i,i,i)
	print insert_1281 %(i,i,i,i,i)
	print insert_1291 %(i,i,i,i,i)
	print insert_1024 %(i,i,i,i,i)
	print insert_1266 %(i,i,i,i,i)
	print insert_1021 %(i,i,i,i,i)
	print insert_1126 %(i,i,i,i,i)
	print insert_1028 %(i,i,i,i,i)
