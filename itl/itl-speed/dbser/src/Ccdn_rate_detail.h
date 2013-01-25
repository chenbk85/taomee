/*
 * =========================================================================
 *
 *        Filename: Ccdn_rate_detail.h
 *
 *        Version:  1.0
 *        Created:  2011-05-18 14:59:58
 *        Description:  w
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#ifndef  CCDN_RATE_DETAIL_H
#define  CCDN_RATE_DETAIL_H

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

#define MAX_DT_CDNIP_TB		100
#define MAX_DT_PROV_TB		100

class Ccdn_rate_detail:public CtableRoute
{
private:
public:
	Ccdn_rate_detail (mysql_interface * db);
	~Ccdn_rate_detail ();
	int	insert_cdnip_tb( cdnrate_ip_isp_in *p_in );
	int	insert_area_tb( cdnrate_ip_isp_in *p_in );
	void write_cdnip_tb_now(int di, int ti);
	void write_area_tb_now(int di, int ti);

    int insert_speed_dis_ip_tb(cdnrate_ip_isp_in *p_in );
    int insert_speed_dis_area_tb(cdnrate_ip_isp_in *p_in );
    void write_speed_dis_ip_tb_now(int di, int ti);
    void write_speed_dis_area_tb_now(int di, int ti);

    int insert_stat_tb(cdnrate_ip_isp_in *p_in);
    void write_stat_tb_now(int di);

	char *cdnipptr[MAX_CDNPRJ_DB][MAX_DT_CDNIP_TB][4];
	char *cdnipstr[MAX_CDNPRJ_DB][MAX_DT_CDNIP_TB][4];//[1024*16];
	int	 cdnipcnt[MAX_CDNPRJ_DB][MAX_DT_CDNIP_TB];

	char *provptr[MAX_CDNPRJ_DB][MAX_DT_PROV_TB][4];
	char *provstr[MAX_CDNPRJ_DB][MAX_DT_PROV_TB][4];//[1024*16];
	int	 provcnt[MAX_CDNPRJ_DB][MAX_DT_PROV_TB];

    char *speed_dis_ipptr[MAX_CDNPRJ_DB][MAX_DT_CDNIP_TB][4];
    char *speed_dis_ipstr[MAX_CDNPRJ_DB][MAX_DT_CDNIP_TB][4];
    int speed_dis_ipcnt[MAX_CDNPRJ_DB][MAX_DT_CDNIP_TB][4];

    char *speed_dis_provptr[MAX_CDNPRJ_DB][MAX_DT_PROV_TB][4];
	char *speed_dis_provstr[MAX_CDNPRJ_DB][MAX_DT_PROV_TB][4];//[1024*16];
	int	 speed_dis_provcnt[MAX_CDNPRJ_DB][MAX_DT_PROV_TB][4];

    char *stat_ptr[MAX_CDNPRJ_DB];
    char *stat_str[MAX_CDNPRJ_DB];//[1024*16];
    int stat_cnt[MAX_CDNPRJ_DB];

};


#endif  /*CCDN_RATE_DETAIL_H*/
