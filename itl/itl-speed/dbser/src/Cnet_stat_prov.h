/*
 * =========================================================================
 *
 *        Filename: Cnet_stat_prov.h
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

#ifndef  CNET_STAT_PROV_H
#define  CNET_STAT_PROV_H


#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

typedef struct netstat_prov{
	 uint32_t        idcid;
	 uint32_t        provid;
	 uint32_t        cityid;
	 char			 comp[64];
     uint32_t        logtime;
     uint32_t        d_v;
     uint32_t        d_c;
     uint32_t        l_v;
     uint32_t        l_c;
     uint32_t        h_v;
     uint32_t        h_c;
} netstat_prov_t;

//定义最大省份 目前只有33个省
#define	 MAX_PROV_TB	100

class Cnet_stat_prov:public CtableRoute
{
private:
public:
	Cnet_stat_prov (mysql_interface * db);
	~Cnet_stat_prov ();
	int	insert( net_stat_prov_report_in *p_in );
	int insert( net_stat_report_prov_delay_in *p_in );
	int insert( net_stat_report_prov_lost_in *p_in );
	int insert( net_stat_report_prov_hop_in *p_in );
	int insert_city_tb (netstat_prov_t* anp);

	void write_city_tb_now(int di, int ti);

	char *cityptr[MAX_IDC_DB][MAX_PROV_TB][4];
	char *citystr[MAX_IDC_DB][MAX_PROV_TB][4];//[1024*16];
	int	 citycnt[MAX_IDC_DB][MAX_PROV_TB];
};



#endif  /*CNET_STAT_PROV_H*/
