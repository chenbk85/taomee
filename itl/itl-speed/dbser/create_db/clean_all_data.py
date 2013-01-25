#!/usr/bin/python

#cdn rate detail
for prj in [ 1,3,4]:
    print "use cdn_rate_detail_%02d; "%(prj)
    for i in range(4):
        for j in range(100):
            print "truncate table t_cdnrate_cdnip%02d_lv%d;"%(j,i)	

    for j in [ 110000,120000,130000,140000,150000,210000,220000,230000,310000,320000,330000,340000,350000,360000,370000,410000,420000,430000,440000,450000,460000,500000,510000,520000,530000,540000,610000,620000,630000,640000,650000,710000,810000,820000,830000]: 
        for i in range(4):
            print "truncate table t_cdnrate_prov%06d_lv%d;"%(j,i)

#cdn rate
for prj in [ 1,3,4]:
    print "use cdn_rate_%02d; "%(prj)
    for i in range(4):
        for j in range(10):
            print "truncate table cdn_rate_ip%d_lv%d;"%(j,i)
 	
    for i in range(4):
        print "truncate table cdn_rate_node_lv%d;"%(i)

    for i in range(4):
        for j in [ 110000,120000,130000,140000,150000,210000,220000,230000,310000,320000,330000,340000,350000,360000,370000,410000,420000,430000,440000,450000,460000,500000,510000,520000,530000,540000,610000,620000,630000,640000,650000,710000,810000,820000,830000]: 
            print "truncate table cdn_rate_prov_%06u_lv%d;"%(j,i)	

        for i in range(4):
            print "truncate table cdn_rate_prov_lv%d;"%(i)

#cdn rate speed ids
for prj in [ 1,3,4]:
    print "use cdn_rate_speed_dis_%02d; "%(prj)
    for i in range(4):
        for j in range(100):
            print "truncate table t_cdn_speed_dis_cdnip%02d_lv%d;"%(j,i)	

    for j in [ 110000,120000,130000,140000,150000,210000,220000,230000,310000,320000,330000,340000,350000,360000,370000,410000,420000,430000,440000,450000,460000,500000,510000,520000,530000,540000,610000,620000,630000,640000,650000,710000,810000,820000,830000]: 
        for i in range(4):
            print "truncate table t_cdn_speed_dis_prov%06d_lv%d;"%(j,i)

#cdn rate stat
for prj in [ 1,3,4]:
    print "use cdn_rate_%02d; "%(prj)
    print "truncate table t_stat;"

#net stat idc
for prj in [ 1,2,3,4]:
    print "use net_stat_%02d; "%(prj)
    for i in range(4):
        for j in range(10):
            print "truncate table net_stat_ip%d_lv%d;"%(j, i)

    for i in range(4):
        for j in [110000,120000,130000,140000,150000,210000,220000,230000,310000,320000,330000,340000,350000,360000,370000,410000,420000,430000,440000,450000,460000,500000,510000,520000,530000,540000,610000,620000,630000,640000,650000,710000,810000,820000,830000]:
            print "truncate table net_stat_prov_%06u_lv%d;"%(j,i)

#net stat link
print "use link_stat; "
for i in range(4):
	print "truncate table link_stat_lv%d;"%(i)

#url stat
for prj in [ 1,2,3]:
    print "use url_rate_record_%02d; "%(prj)
    for i in range(1000):
        print "truncate table t_record_user_%03u;"%(i)

#url record
for prj in [ 1,2,3]:
    print "use url_stat_%02d; "%(prj)
    for i in range(4):
        print "truncate table t_url_isp_lv%d;"%(i)	

    for i in range(4):
        print "truncate table t_url_prov_lv%d;"%(i)	

    for i in range(4):
        for j in [110000,120000,130000,140000,150000,210000,220000,230000,310000,320000,330000,340000,350000,360000,370000,410000,420000,430000,440000,450000,460000,500000,510000,520000,530000,540000,610000,620000,630000,640000,650000,710000,810000,820000,830000]:
            print "truncate table t_url_prov_%06u_lv%d;"%(j,i)	

