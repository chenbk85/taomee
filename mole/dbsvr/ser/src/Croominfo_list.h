/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_list.h
 *
 *    Description:  把roominfo表内的大字段分解出来
 *
 *        Version:  1.0
 *        Created:  01/19/2010 10:41:18 AM
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Croominfo_list.h
 *
 *         Author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#ifndef roominfo_list_incl
#define roominfo_list_incl

#include <algorithm>
#include <cstring>
#include "CtableRoute10x10.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"

class Croominfo_list : public CtableRoute10x10
{

public:
Croominfo_list(mysql_interface * db );
int insert(userid_t userid);
int get_presentlist(userid_t userid, stru_presentlist *p_out );
int update_presentlist(userid_t userid ,stru_presentlist * p_list );
int presentlist_add(userid_t userid ,stru_present * p_item );

int get_pk_list(userid_t userid, stru_pk_list *p_out );
int update_pk_list( userid_t userid, stru_pk_list *p_list ) ;
int pk_add(userid_t userid, roominfo_pk_add_in *p_in );
int update_wish_sql(userid_t userid, stru_roominfo_wish *p_list); 
int select_wish_sql(userid_t user_id, stru_roominfo_wish *p_out);
};

#endif


