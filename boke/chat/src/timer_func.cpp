/*
 * =====================================================================================
 *
 *       Filename:  timer_func.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/28/2011 08:09:44 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ian (Ian), ian@taomee
 *        Company:  taomee co.
 *
 * =====================================================================================
 */

#include "timer_func.h"
#include "dispatcher.h"
#include "dirty_word.h"
#include "global.h"


#define REGISTER_TIMER_TYPE(nbr_, cb_) \
        do { \
            if (register_timer_callback(nbr_, cb_) == -1) \
                ERROR_RETURN(("register timer type error\t[%u]", nbr_), -1); \
        } while(0)

int clear_msg_timely(void* owner, void* data)
{   
	try_free_msg_timely();
	ADD_ONLINE_TIMER(&g_events, n_clear_msg_timely, 0, 300);
    return 0;
} 

int load_dirty_timely(void* owner, void* data)
{   
	g_dirty_word->reload();
	ADD_ONLINE_TIMER(&g_events, n_load_dirty_timely, 0, g_dirty_word->reload_time);
    return 0;
} 

int load_valid_words(void* owner, void* data)
{   
	std::map<uint32_t, CChatCheck*>::iterator it;
	for (it = game_chat_check.begin(); it != game_chat_check.end(); it ++) {
		CChatCheck* pcheck = it->second;
		pcheck->reload_valid_words();
	}
	ADD_ONLINE_TIMER(&g_events, n_load_valid_words, 0, 600);
    return 0;
} 

int chat_forbid_pop(void* owner, void* data)
{
	DEBUG_LOG("CHAT FORBID TIME UP");
	CChatCheck* pcheck = (CChatCheck*)data;
	if (!pcheck) return 0;

	std::vector<uint32_t> userlist;
	uint32_t addtime = pcheck->chat_forbid.pop_forbid_user(userlist);
	if (addtime) {
		ADD_ONLINE_TIMER(&g_events, n_chat_forbid_pop, pcheck, addtime);
	}

	for (uint32_t loop = 0; loop < userlist.size(); loop ++) {
		pcheck->do_after_forbid(userlist[loop]);
	}
	return 0;
}

int init_all_timer_type()
{   
	INIT_LIST_HEAD(&g_events.timer_list);
    REGISTER_TIMER_TYPE(n_clear_msg_timely, clear_msg_timely);
    REGISTER_TIMER_TYPE(n_load_dirty_timely, load_dirty_timely);
    REGISTER_TIMER_TYPE(n_chat_forbid_pop, chat_forbid_pop);
	REGISTER_TIMER_TYPE(n_load_valid_words, load_valid_words);
    return 0;
}


int regist_timers()
{
	ADD_ONLINE_TIMER(&g_events, n_clear_msg_timely, 0, 300);
	ADD_ONLINE_TIMER(&g_events, n_load_dirty_timely, 0, g_dirty_word->reload_time);
	ADD_ONLINE_TIMER(&g_events, n_load_valid_words, 0, 600);
	return 0;
}
