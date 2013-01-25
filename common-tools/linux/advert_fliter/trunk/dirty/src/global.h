/*
 * =====================================================================================
 *
 *       Filename:  global.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/30/2011 01:22:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ian (), ian@taomee.com
 *        Company:  Taomee.Ltd
 *
 * =====================================================================================
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <algorithm>
#include <string>
#include "dirty_word.h"
#include <libtaomee/list.h>

const int max_game_id = 100;
extern Cdirty_word* g_dirty_word;

typedef struct timer_head {
    list_head_t timer_list;
}timer_head_t;
extern timer_head_t g_events;

#endif
