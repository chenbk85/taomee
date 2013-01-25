#!/usr/bin/python

for prj in [ 1,2,3,4]:
    print "drop database if exists cdn_rate_speed_dis_%02d; "%(prj)
    print "create database cdn_rate_speed_dis_%02d CHARACTER SET 'utf8' COLLATE 'utf8_general_ci'; "%(prj)
    print "use cdn_rate_speed_dis_%02d; "%(prj)
    for i in range(4):
        for j in range(100):

            print """create table t_cdn_speed_dis_cdnip%02d_lv%d(
            ip  integer unsigned,
            node integer unsigned,
            seq  integer unsigned,
            provid  integer unsigned,
            cityid  integer unsigned,
            comp     varchar(64),
            speed_tag integer,
            count  integer unsigned default 0,
            primary key ( ip,node,seq,provid,cityid,comp,speed_tag )
    )default charset utf8;

    """%(j,i)	


    for j in [ 110000,120000,130000,140000,150000,210000,220000,230000,310000,320000,330000,340000,350000,360000,370000,410000,420000,430000,440000,450000,460000,500000,510000,520000,530000,540000,610000,620000,630000,640000,650000,710000,810000,820000,830000]: 
        for i in range(4):
		
            print """create table t_cdn_speed_dis_prov%06d_lv%d(
            ip integer unsigned,
            node integer unsigned,
            cityid  integer unsigned,
            seq integer unsigned,
            comp  varchar(64),
            speed_tag integer,
            count  integer unsigned default 0,
            primary key (ip, node, cityid,seq,comp,speed_tag)
            )default charset utf8;
		
             """%(j,i)

