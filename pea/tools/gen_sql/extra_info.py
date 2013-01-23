#!/usr/bin/env python
# -*- coding: utf-8 -*-

import MySQLdb

def sql(cur):

    cur.execute("set autocommit=0");

    name = "pea_extra_info"

    for i in range(100):
        for j in range(10):
            table = "pea_%02d.%s_%d" % (i, name, j);
            print table;

            cur.execute("drop table if exists %s" % table);
            cur.execute("create table %s(\
            user_id int(10) unsigned not null default 0,\
            role_tm int(10) unsigned not null default 0,\
            server_id int(10) unsigned not null default 0,\
            info_id int(10) unsigned not null default 0, \
            info_value int(10) unsigned not null default 0, \
            primary key (user_id, role_tm, server_id, info_id)\
            ) engine=InnoDB default charset=utf8;" % table)

    cur.execute("commit;")
