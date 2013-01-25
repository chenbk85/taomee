/** 
 * ========================================================================
 * @file Cdb_cdn_info.h
 * @brief 
 * @author elvacheng
 * @version 1.0.0
 * @date 2012-09-14
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
*/
#ifndef  CDB_CDN_INFO_H_2012_09_14
#define  CDB_CDN_INFO_H_2012_09_14

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

#define MAX_DT_NOTE_TB     100
#define MAX_DT_PROV_TB     100

class Cdb_cdn_info:public CtableRoute
{

public:
    Cdb_cdn_info(mysql_interface * db);
    ~Cdb_cdn_info();
    
    /** 
     * @brief 更新cdn和表中的信息
     * 
     * @param p_in
     * 
     * @return 
     */
    int insert_tables(cdnrate_ip_isp_in *p_in);

private:
    int insert_node_list_tb( cdnrate_ip_isp_in *p_in );
    int write_node_list_tb_now( int di );

    int insert_node_avg_tb( cdnrate_ip_isp_in *p_in );
    int write_node_avg_tb_now( int di, int ti );

    int insert_node_visit_tb( cdnrate_ip_isp_in *p_in );
    int write_node_visit_tb_now( int di, int ti );

    int insert_ip_detail_tb( cdnrate_ip_isp_in *p_in );
    int write_ip_detail_tb_now( int di, int ti );

    int insert_prov_tb( cdnrate_ip_isp_in *p_in );
    int write_prov_tb_now( int di, int ti );

    int insert_stat_tb(cdnrate_ip_isp_in *p_in);
    int write_stat_tb_now(int di);

private:

    // 数据库的数据缓存
    char * str_node_list[MAX_CDNPRJ_DB][4];
    char * ptr_node_list[MAX_CDNPRJ_DB][4];
    int cnt_node_list[MAX_CDNPRJ_DB];
   

    char * str_node_avg[MAX_CDNPRJ_DB][MAX_DT_NOTE_TB][4];
    char * ptr_node_avg[MAX_CDNPRJ_DB][MAX_DT_NOTE_TB][4];
    int cnt_node_avg[MAX_CDNPRJ_DB][MAX_DT_NOTE_TB][4];

    char * str_node_visit[MAX_CDNPRJ_DB][MAX_DT_NOTE_TB][4];
    char * ptr_node_visit[MAX_CDNPRJ_DB][MAX_DT_NOTE_TB][4];
    int  cnt_node_visit[MAX_CDNPRJ_DB][MAX_DT_NOTE_TB][4];
    
    char * str_ip_detail[MAX_CDNPRJ_DB][MAX_DT_NOTE_TB][4];
    char * ptr_ip_detail[MAX_CDNPRJ_DB][MAX_DT_NOTE_TB][4];
    int cnt_ip_detail[MAX_CDNPRJ_DB][MAX_DT_NOTE_TB];

    char * str_prov[MAX_CDNPRJ_DB][MAX_DT_PROV_TB][4];
    char * ptr_prov[MAX_CDNPRJ_DB][MAX_DT_PROV_TB][4];
    int cnt_prov[MAX_CDNPRJ_DB][MAX_DT_PROV_TB][4];

    char * str_stat[MAX_CDNPRJ_DB];
    char * ptr_stat[MAX_CDNPRJ_DB];
    int cnt_stat[MAX_CDNPRJ_DB];

};



#endif  /*CDB_CDN_INFO_H_2012_09_14*/
