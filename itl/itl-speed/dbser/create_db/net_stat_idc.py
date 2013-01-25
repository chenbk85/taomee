#!/usr/bin/python
for prj in [ 1,2,3,4,5,6,7]:
    print "drop database if exists net_stat_%02d; "%(prj)
    print "create database net_stat_%02d CHARACTER SET 'utf8' COLLATE 'utf8_general_ci'; "%(prj)
    print "use net_stat_%02d; "%(prj)
    for i in range(4):
        for j in range(10):
            if i in [0,1]:
                type_str="integer"
            else:
                type_str="bigint"

            print """create table net_stat_ip%d_lv%d(
            ip  integer unsigned,
            seq  integer unsigned,
            delay_v %s unsigned default 0,
            delay_c int unsigned default 0,
            lost_v %s unsigned default 0,
            lost_c int unsigned default 0,
            hop_v %s unsigned default 0,
            hop_c int unsigned default 0,
            primary key ( seq,ip )
    )default charset utf8;

    """%(j, i, type_str, type_str, type_str)

    for i in range(4):
        for j in [110000,120000,130000,140000,150000,210000,220000,230000,310000,320000,330000,340000,350000,360000,370000,410000,420000,430000,440000,450000,460000,500000,510000,520000,530000,540000,610000,620000,630000,640000,650000,710000,810000,820000,830000]:
            print """create table net_stat_prov_%06u_lv%d(
            cityid  integer unsigned,
            comp  varchar(64),
            seq  integer unsigned,
            delay_v %s unsigned default 0,
            delay_c int unsigned default 0,
            lost_v %s unsigned default 0,
            lost_c int unsigned default 0,
            hop_v %s unsigned default 0,
            hop_c int unsigned default 0,
            primary key ( seq,cityid,comp)
            )default charset utf8;

            """%(j,i, type_str, type_str, type_str)

