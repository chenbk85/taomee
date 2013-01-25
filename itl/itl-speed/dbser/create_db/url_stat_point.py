#!/usr/bin/python
for prj in [ 1,2,3]:
    print "drop database if exists url_stat_%02d; "%(prj)
    print "create database url_stat_%02d CHARACTER SET 'utf8' COLLATE 'utf8_general_ci'; "%(prj)
    print "use url_stat_%02d; "%(prj)

    for i in range(4):
        if i in [0,1]:
            int_t="integer"
        else:
            int_t="bigint"

        print """create table t_url_isp_lv%d(
        ISPID	int(3) unsigned,
        seq		integer unsigned,
        pageid  integer unsigned,
        comp  varchar(64),
        repot_cnt integer unsigned default 0,
        count  int(3) unsigned default 0,
        v0		%s unsigned default 0,
        v1		%s unsigned default 0,
        v2		%s unsigned default 0,
        v3		%s unsigned default 0,
        v4		%s unsigned default 0,
        v5		%s unsigned default 0,
        v6		%s unsigned default 0,
        v7		%s unsigned default 0,
        v8		%s unsigned default 0,
        v9		%s unsigned default 0,
        primary key (seq,pageid,ISPID)
        )default charset utf8;

        """%(i,int_t,int_t,int_t,int_t,int_t,int_t,int_t,int_t,int_t,int_t)	

    for i in range(4):
        if i in [0,1]:
            int_t="integer"
        else:
            int_t="bigint"

        print """create table t_url_prov_lv%d(
        provid	integer unsigned,
        seq		integer unsigned,
        pageid  integer unsigned,
        repot_cnt integer unsigned default 0,
        count  int(3) unsigned default 0,
        v0		%s unsigned default 0,
        v1		%s unsigned default 0,
        v2		%s unsigned default 0,
        v3		%s unsigned default 0,
        v4		%s unsigned default 0,
        v5		%s unsigned default 0,
        v6		%s unsigned default 0,
        v7		%s unsigned default 0,
        v8		%s unsigned default 0,
        v9		%s unsigned default 0,
        primary key (seq,pageid,provid)
    )default charset utf8;

    """%(i,int_t,int_t,int_t,int_t,int_t,int_t,int_t,int_t,int_t,int_t)	


    for i in range(4):
        for j in [110000,120000,130000,140000,150000,210000,220000,230000,310000,320000,330000,340000,350000,360000,370000,410000,420000,430000,440000,450000,460000,500000,510000,520000,530000,540000,610000,620000,630000,640000,650000,710000,810000,820000,830000]:
            if i in [0,1]:
                int_t="integer"
            else:
                int_t="bigint"
           
            print """create table t_url_prov_%06u_lv%d(
            seq  integer unsigned,
            pageid  integer unsigned,
            ISPID	int(3) unsigned,
            cityid  integer unsigned,
         comp  varchar(64),
         repot_cnt integer unsigned default 0,
         count  int(3) unsigned default 0,
         v0		%s unsigned default 0,
         v1		%s unsigned default 0,
         v2		%s unsigned default 0,
         v3		%s unsigned default 0,
         v4		%s unsigned default 0,
         v5		%s unsigned default 0,
         v6		%s unsigned default 0,
         v7		%s unsigned default 0,
         v8		%s unsigned default 0,
         v9		%s unsigned default 0,
         primary key (seq,pageid,cityid,comp)
         )default charset utf8;

        """%(j,i, int_t,int_t,int_t,int_t,int_t,int_t,int_t,int_t,int_t,int_t)	

