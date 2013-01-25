/*
 * =========================================================================
 *
 *        Filename: Curl_stat_user.h
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

#ifndef  CURL_STAT_USER_H
#define  CURL_STAT_USER_H

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

#define  MAX_URLUSER_TB	1000
#define  MAX_POINT_NUM		10

typedef struct url_rate_record {
	uint32_t                prjid;
    uint32_t                userid;
    uint32_t                time;
    uint32_t                clientip;
	uint32_t                pageid;
	uint32_t				count;
	uint32_t				v[MAX_POINT_NUM];
}url_rate_rcod_t;

class Curl_stat_user:public CtableRoute
{
private:
public:
	Curl_stat_user (mysql_interface * db);
	~Curl_stat_user ();
	int	insert( url_rate_record_in *p_in );
	int	insert_url_record( url_rate_rcod_t *aur );

	void write_record_tb_now(int di, int ti);
	char *urlrecordptr[MAX_URLPRJ_DB][MAX_URLUSER_TB];
	char *urlrecordstr[MAX_URLPRJ_DB][MAX_URLUSER_TB];//[1024*16];
	int	 urcnt[MAX_URLPRJ_DB][MAX_URLUSER_TB];
};



#endif  /*CURL_STAT_USER_H*/
