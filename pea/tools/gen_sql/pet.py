#!/usr/bin/env python
# -*- coding: utf-8 -*-

import MySQLdb

def sql(cur):

    cur.execute("set autocommit=0");

    name = "pea_pet"

    for i in range(100):
        for j in range(10):
            table = "pea_%02d.%s_%d" % (i, name, j);
            print table;

            cur.execute("drop table if exists %s" % table);
            cur.execute("create table %s(\
            pet_no int(10) unsigned not null auto_increment,\
            user_id int(10) unsigned not null default 0,\
            role_tm int(10) unsigned not null default 0,\
            server_id int(10) unsigned not null default 0,\
            pet_id int(10) unsigned not null default 0,\
            pet_name varchar(16) not null default '',\
            level int(10) unsigned not null default 1,\
            exp int(10) unsigned not null default 0,\
            quality int(10) unsigned not null default 1,\
            status int(10) unsigned not null default 3,\
            iq int(10) unsigned not null default 0,\
            rand_attr_1 int(10) unsigned not null default 0,\
            rand_attr_2 int(10) unsigned not null default 0,\
            rand_attr_3 int(10) unsigned not null default 0,\
            rand_attr_4 int(10) unsigned not null default 0,\
            train_attr_1 int(10) unsigned not null default 0, \
            train_attr_2 int(10) unsigned not null default 0, \
            train_attr_3 int(10) unsigned not null default 0, \
            train_attr_4 int(10) unsigned not null default 0, \
            try_train_attr_1 int(10) unsigned not null default 0, \
            try_train_attr_2 int(10) unsigned not null default 0, \
            try_train_attr_3 int(10) unsigned not null default 0, \
            try_train_attr_4 int(10) unsigned not null default 0, \
            primary key (pet_no, user_id, role_tm, server_id, pet_id)\
            ) engine=InnoDB default charset=utf8;" % table)

    cur.execute("commit;")
