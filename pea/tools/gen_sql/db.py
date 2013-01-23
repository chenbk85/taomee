#!/usr/bin/env python
# -*- coding: utf-8 -*-

import MySQLdb

def sql(cur):

    cur.execute("set autocommit=0");


    for i in range(100):
        db = "pea_%02d" % i;
        print db;
        #sql = "drop database if exists %s; set names utf8; create database %s" % (db, db);
        sql = "create database %s" % (db);
        print sql;
        cur.execute(sql);


    cur.execute("commit;")
