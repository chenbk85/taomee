#!/usr/bin/python
# -*- coding: utf-8 -*-

for i in range(100):
	for j in range( 10):	
		print """
		drop table POP_%02d.t_user_log_%01d;
		create table IF NOT EXISTS  POP_%02d.t_user_log_%01d 
 (
  userid  int(10) unsigned NOT NULL,
  logtime int(10) unsigned NOT NULL,
  v1   	int(10) unsigned NOT NULL,
  v2   	int(10) unsigned NOT NULL,
  KEY userid(userid)
) ENGINE=InnoDB ;
		"""%	(i,j,i,j)
