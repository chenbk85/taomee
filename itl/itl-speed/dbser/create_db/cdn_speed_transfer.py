#!/usr/bin/python

import sys
import MySQLdb

red_clr = "\033[31m"
grn_clr = "\033[32m"
end_clr = "\033[0m"

db_conf = {
    'host':'localhost',
    'user':'root',
    'passwd':'oaadmin',
    'db':'information_schema'
}
g_project_ids = (1, 3, 4)
g_province_ids = (110000,120000,130000,140000,150000,210000,220000,230000,310000,320000,330000,340000,350000,360000,370000,410000,420000,430000,440000,450000,460000,500000,510000,520000,530000,540000,610000,620000,630000,640000,650000,710000,810000,820000,830000)


def db_transfer_node_list(cursor):
    global g_project_ids
    for prj_id in g_project_ids:
        src_db_name = "cdn_rate_detail_%02d" % (prj_id)
        dst_db_name = "db_cdn_info_%02d" % (prj_id)
        for level in range(4):
            dst_table_name = "t_node_list_lv%d" % (level)
            for node in range(100):
                src_table_name = "t_cdnrate_cdnip%02d_lv%d" % (node, level)
                db_sql = "SELECT DISTINCT seq, node, ip FROM %s.%s" % (src_db_name, src_table_name)
                try:
                    cursor.execute(db_sql)
                except Exception, e:
                    print "%s----cursor execute sql[%s] failed.----%s" % (red_clr, db_sql, end_clr)
                    print e
                    exit(-1)

                sql_results = cursor.fetchall()
                for result in sql_results:
                    seq = result[0]
                    node = result[1]
                    ip = result[2]
                    insert_sql = "INSERT INTO %s.%s SET seq = %s, node = %s, ip = %s"\
                            % (dst_db_name, dst_table_name, seq, node, ip)
                    print insert_sql
                    #try:
                    #    cursor.execute(insert_sql);
                    #except Exception, e:
                    #    print "%s----cursor execute sql[%s] failed.----%s" % (red_clr, insert_sql, end_clr)
                    #    print e
                        

def db_transfer_node_avg(cursor):
    global g_project_ids
    for prj_id in g_project_ids:
        src_db_name = "cdn_rate_detail_%02d" % (prj_id)
        dst_db_name = "db_cdn_info_%02d" % (prj_id)
        for level in range(4):
            for node in range(100):
                src_table_name = "t_cdnrate_cdnip%02d_lv%d" % (node, level)
                dst_table_name = "t_node%02d_avg_lv%d" % (node, level)
                db_sql = "SELECT seq, node, SUM(value), SUM(count) FROM %s.%s GROUP BY node, seq"\
                        % (src_db_name, src_table_name)
                try:
                    cursor.execute(db_sql)
                except Exception, e:
                    print "%s----cursor execute sql[%s] failed.----%s" % (red_clr, db_sql, end_clr)
                    print e
                    exit(-1)

                sql_results = cursor.fetchall()
                for result in sql_results:
                    seq = result[0]
                    node = result[1]
                    value_sum = result[2]
                    count_sum = result[3]
                    insert_sql = "INSERT INTO %s.%s SET seq = %s, node = %s, value_sum = %s, count_sum = %s"\
                            % (dst_db_name, dst_table_name, seq, node, value_sum, count_sum)
                    for speed in range(32):
                        if speed > 30:
                            db_sql = "SELECT SUM(count) FROM %s.%s"\
                                    " WHERE seq = %s AND node = %s AND FLOOR(value/10000) >= %d"\
                                    % (src_db_name, src_table_name, seq, node, speed)
                        else:
                            db_sql = "SELECT SUM(count) FROM %s.%s"\
                                    " WHERE seq = %s AND node = %s AND FLOOR(value/10000) = %d"\
                                    % (src_db_name, src_table_name, seq, node, speed)
                        try:
                            cursor.execute(db_sql);
                        except Exception, e:
                            #print "%s----cursor execute sql[%s] failed.----%s" % (red_clr, db_sql, end_clr)
                            print e
                            exit(-1)
                        speed_count_result = cursor.fetchall()
                        if (speed_count_result[0][0] != None):
                            speed_count = int(speed_count_result[0][0])
                        else:
                            speed_count = 0

                        if speed > 30:
                            insert_sql = "%s, speed_over30_count = %s" % (insert_sql, speed_count)
                        else:
                            insert_sql = "%s, speed%d_count = %s" % (insert_sql, speed, speed_count)
                    print insert_sql
                    #try:
                    #    cursor.execute(db_sql);
                    #except Exception, e:
                    #    #print "%s----cursor execute sql[%s] failed.----%s" % (red_clr, db_sql, end_clr)
                    #    print e


def db_transfer_node_visit(cursor):
    global g_project_ids
    global g_province_ids
    for prj_id in g_project_ids:
        src_db_name = "cdn_rate_detail_%02d" % (prj_id)
        dst_db_name = "db_cdn_info_%02d" % (prj_id)
        for level in range(4):
            for province_id in g_province_ids:
                src_table_name = "t_cdnrate_prov%d_lv%d" % (province_id, level)
                db_sql = "SELECT node, seq, comp, SUM(value), SUM(count) FROM %s.%s GROUP BY node, seq, comp"\
                        % (src_db_name, src_table_name)
                try:
                    cursor.execute(db_sql)
                except Exception, e:
                    print "%s----cursor execute sql[%s] failed.----%s" % (red_clr, db_sql, end_clr)
                    print e
                    exit(-1)

                sql_results = cursor.fetchall()
                for result in sql_results:
                    node = result[0]
                    seq = result[1]
                    comp = result[2]
                    value_sum = result[3]
                    count_sum = result[4]
                    dst_table_name = "t_node%02d_visit_lv%d" % (node % 100, level)

                    insert_sql = "INSERT INTO %s.%s SET prov_id = %d, node = %s, seq = %s,"\
                            " comp = '%s', value_sum = %s, count_sum = %s"\
                            % (dst_db_name, dst_table_name, province_id, node, seq, comp, value_sum, count_sum)
                    for speed in range(32):
                        if speed > 30:
                            db_sql = "SELECT SUM(count) FROM %s.%s"\
                                    " WHERE node = %s AND seq = %s AND comp = '%s' AND FLOOR(value/10000) >= %d"\
                                    % (src_db_name, src_table_name, node, seq, comp, speed)
                        else:
                            db_sql = "SELECT SUM(count) FROM %s.%s"\
                                    " WHERE node = %s AND seq = %s AND comp = '%s' AND FLOOR(value/10000) = %d"\
                                    % (src_db_name, src_table_name, node, seq, comp, speed)
                        try:
                            cursor.execute(db_sql);
                        except Exception, e:
                            #print "%s----cursor execute sql[%s] failed.----%s" % (red_clr, db_sql, end_clr)
                            print e
                            exit(-1)
                        speed_count_result = cursor.fetchall()
                        if (speed_count_result[0][0] != None):
                            speed_count = int(speed_count_result[0][0])
                        else:
                            speed_count = 0

                        if speed > 30:
                            insert_sql = "%s, speed_over30_count = %s" % (insert_sql, speed_count)
                        else:
                            insert_sql = "%s, speed%d_count = %s" % (insert_sql, speed, speed_count)
                    print insert_sql
                    #try:
                    #    cursor.execute(db_sql);
                    #except Exception, e:
                    #    #print "%s----cursor execute sql[%s] failed.----%s" % (red_clr, db_sql, end_clr)
                    #    print e


def db_transfer_ip_detail(cursor):
    global g_project_ids
    for prj_id in g_project_ids:
        src_db_name = "cdn_rate_detail_%02d" % (prj_id)
        dst_db_name = "db_cdn_info_%02d" % (prj_id)
        for level in range(4):
            for node in range(100):
                src_table_name = "t_cdnrate_cdnip%02d_lv%d" % (node, level)
                dst_table_name = "t_node%02d_avg_lv%d" % (node, level)
                db_sql = "SELECT ip, seq, SUM(value), SUM(count) FROM %s.%s GROUP BY ip, seq"\
                        % (src_db_name, src_table_name)
                try:
                    cursor.execute(db_sql)
                except Exception, e:
                    print "%s----cursor execute sql[%s] failed.----%s" % (red_clr, db_sql, end_clr)
                    print e
                    exit(-1)

                sql_results = cursor.fetchall()
                for result in sql_results:
                    ip = result[0]
                    seq = result[1]
                    value_sum = result[2]
                    count_sum = result[3]
                    insert_sql = "INSERT INTO %s.%s SET ip = %s, seq = %s, value_sum = %s, count_sum = %s"\
                            % (dst_db_name, dst_table_name, ip, seq, value_sum, count_sum)
                    for speed in range(32):
                        if speed > 30:
                            db_sql = "SELECT SUM(count) FROM %s.%s"\
                                    " WHERE ip = %s AND seq = %s AND FLOOR(value/10000) >= %d"\
                                    % (src_db_name, src_table_name, ip, seq, speed)
                        else:
                            db_sql = "SELECT SUM(count) FROM %s.%s"\
                                    " WHERE ip = %s AND seq = %s AND FLOOR(value/10000) = %d"\
                                    % (src_db_name, src_table_name, ip, seq, speed)
                        try:
                            cursor.execute(db_sql);
                        except Exception, e:
                            #print "%s----cursor execute sql[%s] failed.----%s" % (red_clr, db_sql, end_clr)
                            print e
                            exit(-1)
                        speed_count_result = cursor.fetchall()
                        if (speed_count_result[0][0] != None):
                            speed_count = int(speed_count_result[0][0])
                        else:
                            speed_count = 0

                        if speed > 30:
                            insert_sql = "%s, speed_over30_count = %s" % (insert_sql, speed_count)
                        else:
                            insert_sql = "%s, speed%d_count = %s" % (insert_sql, speed, speed_count)
                    print insert_sql
                    #try:
                    #    cursor.execute(db_sql);
                    #except Exception, e:
                    #    #print "%s----cursor execute sql[%s] failed.----%s" % (red_clr, db_sql, end_clr)
                    #    print e

def db_transfer_province(cursor):
    global g_project_ids
    global g_province_ids
    for prj_id in g_project_ids:
        src_db_name = "cdn_rate_detail_%02d" % (prj_id)
        dst_db_name = "db_cdn_info_%02d" % (prj_id)
        for level in range(4):
            for province_id in g_province_ids:
                src_table_name = "t_cdnrate_prov%d_lv%d" % (province_id, level)
                db_sql = "SELECT seq, comp, SUM(value), SUM(count) FROM %s.%s GROUP BY seq, comp"\
                        % (src_db_name, src_table_name)
                try:
                    cursor.execute(db_sql)
                except Exception, e:
                    print "%s----cursor execute sql[%s] failed.----%s" % (red_clr, db_sql, end_clr)
                    print e
                    exit(-1)

                sql_results = cursor.fetchall()
                for result in sql_results:
                    seq = result[0]
                    comp = result[1]
                    value_sum = result[2]
                    count_sum = result[3]
                    dst_table_name = "t_prov%d_lv%d" % (province_id, level)

                    insert_sql = "INSERT INTO %s.%s SET seq = %d, comp = '%s', value_sum = %s, count_sum = %s"\
                            % (dst_db_name, dst_table_name, seq, comp, value_sum, count_sum)
                    for speed in range(32):
                        if speed > 30:
                            db_sql = "SELECT SUM(count) FROM %s.%s"\
                                    " WHERE seq = %s AND comp = '%s' AND FLOOR(value/10000) >= %d"\
                                    % (src_db_name, src_table_name, seq, comp, speed)
                        else:
                            db_sql = "SELECT SUM(count) FROM %s.%s"\
                                    " WHERE seq = %s AND comp = '%s' AND FLOOR(value/10000) = %d"\
                                    % (src_db_name, src_table_name, seq, comp, speed)
                        try:
                            cursor.execute(db_sql);
                        except Exception, e:
                            #print "%s----cursor execute sql[%s] failed.----%s" % (red_clr, db_sql, end_clr)
                            print e
                            exit(-1)
                        speed_count_result = cursor.fetchall()
                        if (speed_count_result[0][0] != None):
                            speed_count = int(speed_count_result[0][0])
                        else:
                            speed_count = 0

                        if speed > 30:
                            insert_sql = "%s, speed_over30_count = %s" % (insert_sql, speed_count)
                        else:
                            insert_sql = "%s, speed%d_count = %s" % (insert_sql, speed, speed_count)
                    print insert_sql
                    #try:
                    #    cursor.execute(db_sql);
                    #except Exception, e:
                    #    #print "%s----cursor execute sql[%s] failed.----%s" % (red_clr, db_sql, end_clr)
                    #    print e


try:
    db_conn = MySQLdb.connect(host = db_conf['host'],
                user = db_conf['user'],
                passwd = db_conf['passwd'],
                db = db_conf['db'])
except:
    print "%s----connect to db[%s] failed.----%s" % (red_clr, db_conf['db'], end_clr)
    exit(-1)

print "%s----connect to db[%s] succ.----%s" % (grn_clr, db_conf['db'], end_clr)
db_cursor = db_conn.cursor()
db_cursor.execute("SET NAMES UTF8")

db_transfer_node_list(db_cursor)
db_transfer_node_avg(db_cursor)
db_transfer_node_visit(db_cursor)
db_transfer_ip_detail(db_cursor)
db_transfer_province(db_cursor)


db_cursor.close()
