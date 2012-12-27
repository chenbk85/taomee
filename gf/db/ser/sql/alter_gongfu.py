#!/usr/bin/python
#
#gongfu table (gongfupai)
#
#

#t_gongfu="ALTER TABLE `t_gongfu_%01d` DROP COLUMN start_time;"

#t_gongfu1="ALTER TABLE `t_gongfu_%01d` DROP COLUMN friendlist;"
#t_gongfu2="ALTER TABLE `t_gongfu_%01d` DROP COLUMN blacklist;"

#t_gongfu1="ALTER TABLE `t_gongfu_%01d` ADD friendlist varbinary(804) NOT NULL DEFAULT '0';"
#t_gongfu2="ALTER TABLE `t_gongfu_%01d` ADD blacklist varbinary(804) NOT NULL DEFAULT '0';"

#t_gongfu3="ALTER TABLE `t_gongfu_%01d` ADD role_type int(10) unsigned NOT NULL ;"
#t_gongfu4="ALTER TABLE `t_gongfu_%01d` ADD role_regtime int(10) unsigned NOT NULL ;"
#t_gongfu5="update `t_gongfu_%01d` set role_regtime = 1234213411;"
#t_gongfu6="alter table t_gongfu_%01d drop primary key;"
#t_gongfu7="alter table t_gongfu_%01d add primary key (userid,role_regtime);"
#t_gongfu5="update `t_gongfu_%01d` set role_type = 1;"

###############################
t_gongfu1="ALTER TABLE `t_gongfu_%01d` DROP COLUMN nick;"
t_gongfu2="ALTER TABLE `t_gongfu_%01d` DROP COLUMN color;"
t_gongfu3="ALTER TABLE `t_gongfu_%01d` DROP COLUMN xiaomee;"
t_gongfu4="ALTER TABLE `t_gongfu_%01d` DROP COLUMN friendlist;"
t_gongfu5="ALTER TABLE `t_gongfu_%01d` DROP COLUMN blacklist;"
t_gongfu6="ALTER TABLE `t_gongfu_%01d` DROP COLUMN role_type;"
t_gongfu7="ALTER TABLE `t_gongfu_%01d` DROP COLUMN role_regtime;"
t_gongfu8="ALTER TABLE `t_gongfu_%01d` ADD msglist varchar(2008) DEFAULT '';"

for db in range(100):
	print "USE GF_%02d;"%(db)	
	for t in range(10):
		print t_gongfu1 %(t)
		print t_gongfu2 %(t)
		print t_gongfu3 %(t)
		print t_gongfu4 %(t)
		print t_gongfu5 %(t)
		print t_gongfu6 %(t)
		print t_gongfu7 %(t)
		print t_gongfu8 %(t)
