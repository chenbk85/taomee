#!/usr/bin/env python
# -*- coding: utf-8 -*-

import MySQLdb

def sql(cur):

    cur.execute("set autocommit=0");

    name = "pea_user"

    for i in range(100):
        for j in range(10):
            table = "pea_%02d.%s_%d" % (i, name, j);
            print table;

            cur.execute("drop table if exists %s" % table);
            cur.execute("create table %s(\
            user_id int(10) unsigned not null default 0, \
            role_tm int(10) unsigned not null default 0, \
            nick varchar(16) not null default '', \
            eye_model int(10) unsigned not null default 0,\
            resource_id int(10) unsigned not null default 0,\
            server_id int(10) unsigned not null default 0, \
            last_login_tm int(10) unsigned not null default 0, \
            last_off_line_tm int(10) unsigned not null default 0, \
            exp int(10) unsigned not null default 0, \
            level int(10) unsigned not null default 1, \
            max_bag_grid_count int(10) unsigned not null default 100, \
            map_id int(10) unsigned not null default 10, \
            map_x int(10) unsigned not null default 460, \
            map_y int(10) unsigned not null default 540, \
            gold int(10) unsigned not null default 0, \
			forbid_friends_me tinyint(2) unsigned not null default 0, \
			union_id int(10) unsigned not null default 0, \
			offline_msg varbinary(2048) not null default '\0\0\0\0', \
            primary key (user_id, role_tm, server_id) \
            ) engine=InnoDB default charset=utf8;" % table)


    cur.execute("commit;")
