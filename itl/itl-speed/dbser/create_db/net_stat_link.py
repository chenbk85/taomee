#!/usr/bin/python
print "drop database if exists link_stat ; "
print "create database link_stat CHARACTER SET 'utf8' COLLATE 'utf8_general_ci'; "
print "use link_stat; "
for i in range(4):
	if i in [0,1]:
		type_str="integer"
	else:
		type_str="bigint"

	print """create table link_stat_lv%d(
	link integer unsigned,
	seq  integer unsigned,
	delay_v  %s unsigned default 0,
	delay_c  int unsigned default 0,
	lost_v %s unsigned default 0,
	lost_c int unsigned default 0,
	hop_v %s unsigned default 0,
	hop_c int unsigned default 0,
	primary key ( seq,link )
)default charset utf8;
		
 """%(i, type_str, type_str, type_str)
