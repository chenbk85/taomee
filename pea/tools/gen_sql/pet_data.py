#!/usr/bin/env python
# -*- coding: utf-8 -*-

import MySQLdb

def sql(cur):

    cur.execute("set autocommit=0");

    name = "pea_pet"

    for i in range(100):
        for j in range(10):
            table = "pea_%02d.%s_%d" % (i, name, j);
            user_id = 100000 + j * 100 + i;
            pet_id = 1001 + user_id % 8;
            print user_id, table;
            cur.execute("delete from %s" % table);

            cur.execute("insert ignore into %s (user_id, role_tm, server_id, pet_id, level, exp, status, rand_attr_1, rand_attr_2, rand_attr_3, rand_attr_4) values(%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u);" % (table, user_id, 0, 9, pet_id, 1, 10, 1, 10, 10, 10, 10))
    #cur.execute("insert into pea_37." + name + "_16 (user_id, nick, server_id, last_login_tm, last_off_line_tm, level, max_bag_grid_count, map_id, map_x, map_y, gold) values(101637, 'sonic', 9, 123, 123, 1, 100, 1, 100, 100, 100), (201637, 'malong', 9, 123, 123, 1, 100, 10, 100, 100, 100);")



    cur.execute("commit;")
