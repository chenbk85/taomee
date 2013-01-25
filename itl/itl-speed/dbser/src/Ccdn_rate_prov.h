/*
 * =========================================================================
 *
 *        Filename: Ccdn_rate_prov.h
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

#ifndef  CCDN_RATE_PROV_H
#define  CCDN_RATE_PROV_H


#include <map>

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

//定义最大省份 目前只有33个省
#define	 MAX_CDNPROV_TB	100


class Ccdn_rate_prov:public CtableRoute
{
private:
public:
	Ccdn_rate_prov (mysql_interface * db);
	~Ccdn_rate_prov ();
	int	insert_city_tb( cdn_report_prov_in *p_in );
	int insert_prov_tb( cdn_report_prov_in *p_in );

	void write_city_tb_now(int di, int ti);
	void write_prov_tb_now(int di);

	char *cityptr[MAX_CDNPRJ_DB][MAX_CDNPROV_TB][4];
	char *citystr[MAX_CDNPRJ_DB][MAX_CDNPROV_TB][4];//[1024*16];
	int	 citycnt[MAX_CDNPRJ_DB][MAX_CDNPROV_TB];

	char *provptr[MAX_CDNPRJ_DB][4];
	char *provstr[MAX_CDNPRJ_DB][4];//[1024*16];
	int	 provcnt[MAX_CDNPRJ_DB];
};


#endif  /*CCDN_RATE_PROV_H*/
