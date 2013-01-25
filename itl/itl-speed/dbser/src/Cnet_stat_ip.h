/*
 * =========================================================================
 *
 *        Filename: Cnet_stat_ip.h
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

#ifndef  CNET_STAT_IP_H
#define  CNET_STAT_IP_H

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include <stdint.h>

#define MAX_IP_TB	10

typedef struct netstat_ip {
	 uint32_t        id;
	 uint32_t        logtime;
	 uint32_t        d_v;
	 uint32_t        d_c;
	 uint32_t        l_v;
	 uint32_t        l_c;
	 uint32_t        h_v;
	 uint32_t        h_c;
} netstat_ip_t;

class Cnet_stat_ip:public CtableRoute
{
private:
public:
	Cnet_stat_ip (mysql_interface * db);
	~Cnet_stat_ip ();
	int	insert( net_stat_ip_report_in *p_in );

	int insert (int db_idx, int idx, netstat_ip_t* ani);
	int	insert( net_stat_report_ip_delay_in *p_in );
	int insert( net_stat_report_ip_lost_in *p_in );
	int insert( net_stat_report_ip_hop_in *p_in );
	
	void write_ip_tb_now(int di, int ti);

	char *niptr[MAX_IDC_DB][MAX_IP_TB][4];
	char *nistr[MAX_IDC_DB][MAX_IP_TB][4];//[1024*16];
	int	 nicnt[MAX_IDC_DB][MAX_IP_TB];
};



#endif  /*CNET_STAT_IP_H*/
