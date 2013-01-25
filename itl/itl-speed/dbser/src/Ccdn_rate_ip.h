/*
 * =========================================================================
 *
 *        Filename: Ccdn_rate_ip.h
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

#ifndef  CCDN_RATE_IP_H
#define  CCDN_RATE_IP_H

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

#define MAX_CDNIP_TB	10

class Ccdn_rate_ip:public CtableRoute
{
private:
public:
	Ccdn_rate_ip (mysql_interface * db);
	~Ccdn_rate_ip();
	int	insert_ip_tb( cdn_report_ip_in *p_in );
	int	insert_node_tb( cdn_report_ip_in *p_in );

	void write_ip_tb_now(int di, int ti);
	void write_node_tb_now(int di);

	char *ipptr[MAX_CDNPRJ_DB][MAX_CDNIP_TB][4];
	char *ipstr[MAX_CDNPRJ_DB][MAX_CDNIP_TB][4];//[1024*16];
	int	 ipcnt[MAX_CDNPRJ_DB][MAX_CDNIP_TB];

	char* nodeptr[MAX_CDNPRJ_DB][4];
	char* nodestr[MAX_CDNPRJ_DB][4];
	int   nodecnt[MAX_CDNPRJ_DB];
};



#endif  /*CCDN_RATE_IP_H*/
