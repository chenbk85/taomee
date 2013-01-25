#!/usr/bin/python
# -*- coding: utf-8 -*-

for i in range(100):
	print "create  database POP_%02d;"%(i)

#user
for i in range(100):
	for j in range( 10):	
		print """
create table   POP_%02d.t_user_%01d (
	  userid int(10) unsigned NOT NULL,
	  flag 	int(10) unsigned NOT NULL,
	  register_time int(10) unsigned NOT NULL,
	  sex 	int(1) unsigned NOT NULL,
	  age  	int(3) unsigned NOT NULL,
	  nick 	char(16) NOT NULL,
	  PRIMARY KEY  (userid) 
) ENGINE=InnoDB;
		"""%	(i,j)

		print """
create table   POP_%02d.t_task_%01d (
	  userid int(10) unsigned NOT NULL,
	  taskid int(10) unsigned NOT NULL,
	  task_nodeid int(10) unsigned NOT NULL,
	  PRIMARY KEY  (userid,taskid,task_nodeid) 
) ENGINE=InnoDB;
		"""%(i,j)

		print """
create table   POP_%02d.t_item_%01d (
	  userid 	int(10) unsigned NOT NULL,
	  itemid 	int(10) unsigned NOT NULL,
	  usecount 	int(10) unsigned NOT NULL,
	  count 	int(10) unsigned NOT NULL,
	  PRIMARY KEY  (userid,itemid ) 
) ENGINE=InnoDB;
		"""%(i,j)
