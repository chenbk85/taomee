/*
 * =====================================================================================
 *
 *       Filename:  util.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/14/2011 04:10:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef MOLE2_DB_UTIL
#define MOLE2_DB_UTIL

#include <libtaomee++/utils/tcpip.h> 
#include "proto/mole2_db.h" 

extern  Cudp_sender*  g_p_change_log_udp_sender;
void game_change_add( stru_game_change_add &log_item );
#endif
