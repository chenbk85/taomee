#!/usr/bin/python

for prj in [ 1,2,3,4]:
    print "drop database if exists cdn_rate_%02d; "%(prj)
    print "create database cdn_rate_%02d CHARACTER SET 'utf8' COLLATE 'utf8_general_ci'; "%(prj)
    print "use cdn_rate_%02d; "%(prj)
    for i in range(4):
        for j in range(10):
            if i in [ 0,1 ]:
                int_type_str="integer"
            else:
                int_type_str="bigint"

            print """create table cdn_rate_ip%d_lv%d(
            ip  integer unsigned,
            node  integer unsigned,
            seq  integer unsigned,
            value  %s unsigned,
            count  integer unsigned,
            primary key (seq,ip,node )
    )default charset utf8;

    """%(j,i,int_type_str)
 	
    for i in range(4):
        if i in [ 0,1 ]:
            int_type_str="integer"
        else:
            int_type_str="bigint"

        print """create table cdn_rate_node_lv%d(
        node  integer unsigned,
        seq  integer unsigned,
        value  %s unsigned,
        count  integer unsigned,
        primary key (seq,node )
        )default charset utf8;

        """%(i,int_type_str)



    for i in range(4):
        for j in [ 110000,120000,130000,140000,150000,210000,220000,230000,310000,320000,330000,340000,350000,360000,370000,410000,420000,430000,440000,450000,460000,500000,510000,520000,530000,540000,610000,620000,630000,640000,650000,710000,810000,820000,830000]: 
            if i in [ 0,1 ]:
                int_type_str="integer"
            else:
                int_type_str="bigint"

            print """create table cdn_rate_prov_%06u_lv%d(
            cityid  integer unsigned,
            comp  varchar(64),
            seq  integer unsigned,
            value  %s unsigned,
            count  integer unsigned,
            primary key (seq,cityid,comp)
            )default charset utf8;

            """%(j,i,int_type_str)	

    for i in range(4):
        if i in [ 0,1 ]:
            int_type_str="integer"
        else:
            int_type_str="bigint"

        print """create table cdn_rate_prov_lv%d(
        provid  integer unsigned,
        seq  integer unsigned,
        value  %s unsigned,
        count  integer unsigned,
        primary key (seq,provid)
        )default charset utf8;

        """%(i,int_type_str)

