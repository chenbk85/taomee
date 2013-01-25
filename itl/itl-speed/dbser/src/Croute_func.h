/*
 * =====================================================================================
 * 
 *       Filename:  dbdeal.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月02日 16时40分57秒 CST
 *        PRIu64
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CROUTE_FUNC_INC
#define  CROUTE_FUNC_INC
#include "Cfunc_route_db_v2.h"
#include    "Ccdn_rate_ip.h"
#include    "Ccdn_rate_prov.h"
#include    "Ccdn_rate_detail.h"
#include    "Cnet_stat_ip.h"
#include    "Cnet_stat_prov.h"
#include    "Clink_stat.h"
#include    "Curl_stat_prov.h"
#include    "Curl_stat_user.h"

#include  "Cdb_cdn_info.h"
/*
 * =====================================================================================
 *        Class:  Croute_func
 *  Description:  
 * =====================================================================================
 */


class Croute_func:public Cfunc_route_db_v2
{
  private:
	Ccdn_rate_ip  cdn_rate_ip;
	Ccdn_rate_prov  cdn_rate_prov;
	Ccdn_rate_detail  cdn_rate_detail;
	Cnet_stat_ip  net_stat_ip;
	Cnet_stat_prov  net_stat_prov;
	Clink_stat  link_stat;
	Curl_stat_prov  url_stat_prov;
	Curl_stat_user  url_stat_user;
    // add elva
    Cdb_cdn_info db_cdn_info;
  public:
	Croute_func (mysql_interface * db); 
#define PROTO_FUNC_DEF(cmd_name)\
	int cmd_name(DEAL_FUNC_ARG);
	#include "./proto/cdn_rate_func_def.h"
}; /* -----  end of class  Croute_func  ----- */

#endif   /* ----- #ifndef CROUTE_FUNC_INC  ----- */



