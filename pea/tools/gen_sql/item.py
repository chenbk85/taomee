#!/usr/bin/env python
# -*- coding: utf-8 -*-

import MySQLdb

def sql(cur):

    cur.execute("set autocommit=0");

    name = "pea_item"

    for i in range(100):
        for j in range(10):
            table = "pea_%02d.%s_%d" % (i, name, j);
            print table;

            cur.execute("drop table if exists %s" % table);
            cur.execute("create table %s(\
            id int(10) unsigned not null auto_increment primary key, \
            user_id int(10) unsigned not null default 0, \
            role_tm int(10) unsigned not null default 0, \
            server_id int(10) unsigned not null default 0, \
            grid_index int(10) unsigned not null default 0,\
            item_id int(10) unsigned not null default 0,\
            item_count int(10) unsigned not null default 0,\
            get_time int(10) unsigned not null default 0,\
			expire_time int(10) unsigned not null default 0,\
			hide tinyint(2) unsigned not null default 0, \
            unique key(user_id, role_tm, server_id, grid_index)\
            ) engine=InnoDB default charset=utf8;" % table)

    cur.execute("commit;")
