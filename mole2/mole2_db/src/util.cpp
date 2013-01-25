/*
 * =====================================================================================
 *
 *       Filename:  util.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/14/2011 04:10:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "util.h"


void game_change_add( stru_game_change_add &log_item)
{
    log_item.gameid= 7;
    log_item.logtime= time(NULL); 
    g_p_change_log_udp_sender->send_db_msg(log_item.userid , 0xF135 , 0, &log_item);
}
