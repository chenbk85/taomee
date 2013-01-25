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

#ifndef  CLINK_STAT_H
#define  CLINK_STAT_H

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include <stdint.h>


typedef struct link_stat {
	 uint32_t        id;
	 uint32_t        logtime;
	 uint32_t        d_v;
	 uint32_t        d_c;
	 uint32_t        l_v;
	 uint32_t        l_c;
	 uint32_t        h_v;
	 uint32_t        h_c;
} link_stat_t;

class Clink_stat:public CtableRoute
{
private:
public:
	Clink_stat (mysql_interface * db);
	~Clink_stat ();

	int	insert( net_stat_link_report_in *p_in );
	int insert( net_stat_report_link_delay_in *p_in );
	int insert( net_stat_report_link_lost_in *p_in );
	int insert( net_stat_report_link_hop_in *p_in );

	int	insert( net_stat_report_idc_delay_in *p_in );
	int	insert( net_stat_report_idc_lost_in *p_in );
	int	insert( net_stat_report_idc_hop_in *p_in );
	int	insert( net_stat_report_all_idc_delay_in *p_in );
	int	insert( net_stat_report_all_idc_lost_in *p_in );
	int	insert( net_stat_report_all_idc_hop_in *p_in );

	int insert (link_stat_t* ani);
	void write_link_tb_now();

	char *mysqlptr[4];
	char *mysqlstr[4];//[1024*16];
	int	 mycount;
};



#endif  /*CLINK_STAT_H*/
