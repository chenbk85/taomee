#!/usr/bin/python
#coding=UTF-8

for prj_id in [1, 3, 4]:
    print "DROP DATABASE IF EXISTS db_cdn_info_%02d;" % (prj_id)
    print "CREATE DATABASE db_cdn_info_%02d;" % (prj_id)
    print "use db_cdn_info_%02d;" % (prj_id)
    
    #记录所有的node
    for level in range(4):
        print """CREATE TABLE `t_node_list_lv%d`(
        `ip` int(10) unsigned NOT NULL default '0',
        `seq` int(10) unsigned NOT NULL default '0',
        `node` int(10) unsigned default NULL,
        PRIMARY KEY (`ip`, `seq`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8;
    
        """ % (level)
    
    
    #记录每个节点每个时间点的平均值及分布值，按照node进行分布
    for node in range(100):
        for level in range(4):
            print "CREATE TABLE `t_node%02d_avg_lv%d`(" % (node, level)
            print "\t`node` int(10) unsigned NOT NULL default '0',"
            print "\t`seq` int(10) unsigned NOT NULL default '0',"
            print "\t`value_sum` int(10) unsigned NOT NULL default '0',"
            print "\t`count_sum` int(10) unsigned NOT NULL default '0',"
    
            for speed in range(31):
                print "\t`speed%d_count` int(10) unsigned NOT NULL default '0' COMMENT '速率为%d*10KB的数量',"\
                        % (speed, speed)
        
            print "\t`speed_over30_count` int(10) unsigned NOT NULL default '0' COMMENT '速率大于30*10KB的数量',"
            print "\tPRIMARY KEY (`node`, `seq`)"
            print ") ENGINE=InnoDB DEFAULT CHARSET=utf8;\n"
    
    #记录来访地区信息
    for node in range(100):
        for level in range(4):
            print "CREATE TABLE `t_node%02d_visit_lv%d`(" % (node, level)
            print "\t`node` int(10) unsigned NOT NULL default '0',"
            print "\t`prov_id` int(10) unsigned NOT NULL default '0',"
            print "\t`seq` int(10) unsigned NOT NULL default '0',"
            print "\t`comp` varchar(64) NOT NULL default '' COMMENT '运营商',"
            print "\t`value_sum` int(10) unsigned NOT NULL default '0',"
            print "\t`count_sum` int(10) unsigned NOT NULL default '0',"
    
            for speed in range(31):
                print "\t`speed%d_count` int(10) unsigned NOT NULL default '0' COMMENT '速率为%d*10KB的数量',"\
                        % (speed, speed)
        
            print "\t`speed_over30_count` int(10) unsigned NOT NULL default '0' COMMENT '速率大于30*10KB的数量',"
            print "\tPRIMARY KEY (`node`, `prov_id`, `seq`, `comp`)"
            print ") ENGINE=InnoDB DEFAULT CHARSET=utf8;\n"
    
    #查看单个IP详情
    for node in range(100):
        for level in range(4):
            print "CREATE TABLE `t_ip%02d_detail_lv%d`(" % (node, level)
            print "\t`node` int(10) unsigned NOT NULL default '0',"
            print "\t`ip` int(10) unsigned NOT NULL default '0',"
            print "\t`seq` int(10) unsigned NOT NULL default '0',"
            print "\t`value_sum` int(10) unsigned NOT NULL default '0',"
            print "\t`count_sum` int(10) unsigned NOT NULL default '0',"
    
            for speed in range(31):
                print "\t`speed%d_count` int(10) unsigned NOT NULL default '0' COMMENT '速率为%d*10KB的数量',"\
                        % (speed, speed)
        
            print "\t`speed_over30_count` int(10) unsigned NOT NULL default '0' COMMENT '速率大于30*10KB的数量',"
            print "\tPRIMARY KEY (`node`, `ip`, `seq`)"
            print ") ENGINE=InnoDB DEFAULT CHARSET=utf8;\n"
    
    #查看地区详情
    for province_id in [110000,120000,130000,140000,150000,210000,220000,230000,310000,320000,330000,340000,350000,360000,370000,410000,420000,430000,440000,450000,460000,500000,510000,520000,530000,540000,610000,620000,630000,640000,650000,710000,810000,820000,830000]: 
        for level in range(4):
            print "CREATE TABLE `t_prov%02d_lv%d`(" % (province_id, level)
            print "\t`seq` int(10) unsigned NOT NULL default '0',"
            print "\t`comp` varchar(64) NOT NULL default '' COMMENT '运营商',"
            print "\t`value_sum` int(10) unsigned NOT NULL default '0',"
            print "\t`count_sum` int(10) unsigned NOT NULL default '0',"
    
            for speed in range(31):
                print "\t`speed%d_count` int(10) unsigned NOT NULL default '0' COMMENT '速率为%d*10KB的数量',"\
                        % (speed, speed)
        
            print "\t`speed_over30_count` int(10) unsigned NOT NULL default '0' COMMENT '速率大于30*10KB的数量',"
            print "\tPRIMARY KEY (`seq`, `comp`)"
            print ") ENGINE=InnoDB DEFAULT CHARSET=utf8;\n"
    
    print "\n\n"
