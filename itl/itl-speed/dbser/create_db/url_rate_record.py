#!/usr/bin/python
for prj in [ 1,2,3]:
    print "drop database if exists url_rate_record_%02d; "%(prj)
    print "create database url_rate_record_%02d CHARACTER SET 'utf8' COLLATE 'utf8_general_ci'; "%(prj)
    print "use url_rate_record_%02d; "%(prj)

    for i in range(1000):
        print """create table t_record_user_%03u(
        userid  integer unsigned,
        time	integer unsigned,
        clientip  integer unsigned,
        pageid  integer unsigned,
        count  int(3) unsigned default 0,
        v0		integer unsigned default 0,
        v1		integer unsigned default 0,
        v2		integer unsigned default 0,
        v3		integer unsigned default 0,
        v4		integer unsigned default 0,
        v5		integer unsigned default 0,
        v6		integer unsigned default 0,
        v7		integer unsigned default 0,
        v8		integer unsigned default 0,
        v9		integer unsigned default 0,
        primary key (userid,time,pageid)
        )default charset utf8;

        """%(i)	

