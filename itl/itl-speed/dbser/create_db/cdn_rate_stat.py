#!/usr/bin/python

for prj in [ 1,2,3,4]:
    print "create database if not exists cdn_rate_%02d CHARACTER SET 'utf8' COLLATE 'utf8_general_ci'; "%(prj)
    print "use cdn_rate_%02d; "%(prj)
    print """create table if not exists t_stat(
    seq  integer unsigned,
    speed_tag  integer,
    count  integer unsigned,
    primary key (seq, speed_tag)
    )default charset utf8;""" 	
