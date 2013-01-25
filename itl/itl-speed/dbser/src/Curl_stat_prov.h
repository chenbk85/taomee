/*
 * =========================================================================
 *
 *        Filename: Curl_stat_prov.h
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

#ifndef  CURL_STAT_PROV_H
#define  CURL_STAT_PROV_H

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

//定义最大省份 目前只有33个省
#define	 MAX_URLPROV_TB		100
#define  MAX_POINT_NUM		10

typedef struct url_rate_info {
	uint32_t                prjid;
	uint32_t                pageid;
    uint32_t                provid;
    uint32_t                cityid;
    uint32_t                ISPID;
    char             		comp[64]; 
    uint32_t                logtime;
	uint32_t				repot_cnt;
	uint32_t				count;
	uint32_t				v[MAX_POINT_NUM];
}url_rate_info_t;

class Curl_stat_prov:public CtableRoute
{
private:
public:
	Curl_stat_prov (mysql_interface * db);
	~Curl_stat_prov ();
	int	insert( url_stat_report_point_time_in *p_in );
	int	insert_url_isp( url_rate_info_t *puri );
	int	insert_url_prov( url_rate_info_t *puri );
	int	insert_url_detail( url_rate_info_t *puri );

	void write_isp_tb_now(int di);
	void write_prov_tb_now(int di);
	void write_detail_tb_now(int di, int ti);
	char *urlispptr[MAX_URLPRJ_DB][4];
	char *urlispstr[MAX_URLPRJ_DB][4];//[1024*16];
	int	 uicnt[MAX_URLPRJ_DB];
	
	char *urlprovptr[MAX_URLPRJ_DB][4];
	char *urlprovstr[MAX_URLPRJ_DB][4];//[1024*16];
	int	 upcnt[MAX_URLPRJ_DB];
	
	char *urldetailptr[MAX_URLPRJ_DB][MAX_URLPROV_TB][4];
	char *urldetailstr[MAX_URLPRJ_DB][MAX_URLPROV_TB][4];//[1024*16];
	int	 udcnt[MAX_URLPRJ_DB][MAX_URLPROV_TB];
};



#endif  /*CURL_STAT_PROV_H*/
