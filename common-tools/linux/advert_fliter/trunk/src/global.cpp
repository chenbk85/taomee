/*
 * =====================================================================================
 *
 *       Filename:  global.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/28/2011 12:32:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ian (Ian), ian@taomee
 *        Company:  taomee co.
 *
 * =====================================================================================
 */

#include "global.h"

std::map<uint32_t, CChatCheck*> game_chat_check;

timer_head_t g_events;
