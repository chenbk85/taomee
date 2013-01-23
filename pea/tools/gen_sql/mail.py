#!/usr/bin/env python
# -*- coding: utf-8 -*-

import MySQLdb

def sql(cur):

    cur.execute("set autocommit=0");

    name = "pea_mail"

    for i in range(100):
        for j in range(10):
            table = "pea_%02d.%s_%d" % (i, name, j);
            print table;

            cur.execute("drop table if exists %s" % table);
            cur.execute("create table %s(\
			mail_id int(10) unsigned not null auto_increment primary key, \
			mail_time int(10) unsigned not null default 0, \
			mail_state int(10) unsigned not null default 0, \
			mail_templet int(10) unsigned not null default 0, \
			mail_type int(10) unsigned not null default 0, \
			server_id int(10) unsigned not null default 0, \
			sender_id int(10) unsigned not null default 0, \
			sender_role_tm int(10) unsigned not null default 0, \
			sender_nick varchar(1024) not null default '', \
			receive_id int(10) unsigned not null default 0, \
			receive_role_tm int(10) unsigned not null default 0, \
			receive_nick varchar(1024) not null default '', \
			mail_title varchar(1024)  not null default '', \
			mail_content varchar(1024) not null default '', \
			numerical_enclosure varchar(1024) not null default '', \
			item_enclosure varchar(1024) not null default '', \
			equip_enclosure varchar(1024) not null default '' \
            ) engine=InnoDB default charset=utf8;" % table)

    cur.execute("commit;")
