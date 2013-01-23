#!/usr/bin/env python
# -*- coding: utf-8 -*-

import MySQLdb

def sql(cur):

    cur.execute("set autocommit=0");

    name = "pea_user"

    for i in range(100):
        for j in range(10):
            table = "pea_%02d.%s_%d" % (i, name, j);
            user_id = 100000 + j * 100 + i;
            print user_id, table;
            #cur.execute("update %s set map_x = 460, map_y = 540;" % table);


            cur.execute("insert ignore into %s (user_id, nick, server_id, last_login_tm, last_off_line_tm, level, max_bag_grid_count, map_id, map_x, map_y, gold) values(%u, 'malong', 9, 123, 123, 1, 100, 10, 460, 540, 100);" % (table, user_id))



    cur.execute("commit;")
