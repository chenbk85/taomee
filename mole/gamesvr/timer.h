#ifndef _TIMER_H_
#define _TIMER_H_

// C89
#include <stdlib.h>
#include <time.h>
// C99
#include <stdint.h>
// Posix
#include <sys/time.h>
// self-defined
#include <libtaomee/list.h>

// export now and tm_cur
extern struct timeval now;
extern struct tm      tm_cur;

typedef uint32_t timer_id_t;
typedef int (*timer_cb_func_t)(void*, void*);

typedef struct timer_struct {
	struct list_head	entry;
	struct list_head	sprite_list;
	timer_id_t		id;
	time_t			expire;
	void*			owner;
	void*			data;
	timer_cb_func_t         function; // 0 indicates a timer is to be deleted
}timer_struct_t ;

typedef struct micro_timer_struct {
	struct list_head	entry;
	struct timeval		tv;
	void 			*arg;
	void (*function)(void *);
} micro_timer_struct_t ;

typedef enum {
	TIMER_replace_unconditionally,
	TIMER_replace_earlier_timer,
	TIMER_replace_later_timer
} timer_op_t;

void setup_timer();
void destroy_timer();

void scan_seconds_timer();
void scan_microseconds_timer();

static inline void
handle_timer()
{
	static time_t last = 0;

	gettimeofday(&now, 0);
	localtime_r(&now.tv_sec, &tm_cur);
	//second timer
	if (last != now.tv_sec) {
		last = now.tv_sec;
		scan_seconds_timer();
	}
	//microseconds timer - not yet needed
	//scan_microseconds_timer ();
}

timer_id_t add_timer_event(list_head_t* l, timer_cb_func_t func, void* owner, void* data, time_t expire);
// operate a timer event
timer_id_t op_timer_event(list_head_t* l, timer_cb_func_t func, void* owner, void* data, time_t expire, timer_op_t flag);

void mod_expire_time(list_head_t* head, timer_id_t tid, time_t exptm);
void remove_timer(list_head_t* head, timer_id_t tid);
void remove_timers(list_head_t* head);

micro_timer_struct_t *
add_micro_event(void (*function)(void *), struct timeval *tv, void *);

/*--------------------------------------------------
  |          micros to ease your life
  --------------------------------------------------*/
// add a timer event
#define ADD_TIMER_EVENT(owner_, func_, data_, expire_) \
	add_timer_event(&((owner_)->timer_list), (func_), (owner_), (data_), (expire_))
// mod `owner's` timer whose id == `tid`
#define MOD_EXPIRE_TIME(owner_, tid_, exptm_) \
	mod_expire_time(&((owner_)->timer_list), (tid_), (exptm_))
// modify expire time of `owner's` `func` timer event anyhow
#define MOD_EVENT_EXPIRE_TIME(owner_, func_, expire_) \
	op_timer_event(&((owner_)->timer_list), (func_), (owner_), 0, (expire_), TIMER_replace_unconditionally)
// remove `owner's` timer whose id == tid
#define REMOVE_TIMER(owner_, tid_) \
	remove_timer(&((owner_)->timer_list), (tid_))
// remove all timer events of `owner's`
#define REMOVE_TIMERS(owner_) \
	remove_timers(&((owner_)->timer_list))

static inline const struct timeval*
get_now_tv()
{
	return &now;
}

static inline const struct tm*
get_now_tm()
{
	return &tm_cur;
}

#endif
