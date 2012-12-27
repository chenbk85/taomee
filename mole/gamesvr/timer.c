#include <glib.h>

#include <libtaomee/log.h>
#include "timer.h"

#define TIMER_VEC_SIZE		5

struct tvec_root_s {
	struct list_head head;
	int expire;
	time_t min_expiring_time;
};

static struct tvec_root_s vec[TIMER_VEC_SIZE];
static struct list_head micro_timer;

struct timeval now;
struct tm      tm_cur;

/*------------------------------------------
  *  inline function declarations
  *-----------------------------------------*/
static inline void add_timer(timer_struct_t *t);
static inline void remove_micro_timer(micro_timer_struct_t *t);

static inline void do_remove_timer(timer_struct_t* t, int freed);
static inline void do_mod_expire_time(timer_struct_t *p, time_t t);

static inline int  find_min_idx(time_t diff, int max_idx);
static inline void set_min_exptm(time_t exptm, int idx);

static inline timer_struct_t* find_timer(list_head_t* l, timer_cb_func_t func);
static inline timer_struct_t* find_timer_expires_at(list_head_t* l, timer_cb_func_t func, time_t expire);


/*------------------------------------------
  *  non-inline function definations
  *-----------------------------------------*/
void setup_timer ()
{
	int i;

	gettimeofday(&now, 0);
	for (i = 0; i < TIMER_VEC_SIZE; i++) {
		INIT_LIST_HEAD (&vec[i].head);
		vec[i].expire = 1 << (i + 2);
	}
	INIT_LIST_HEAD (&micro_timer);
}

// operate a timer event
timer_id_t
op_timer_event(list_head_t* l, timer_cb_func_t func, void* owner, void* data, time_t expire, timer_op_t flag)
{
	timer_struct_t *timer;

	// TODO - seems like it only finds one event and mod it if condition met
	timer = find_timer(l, func);
	if (!timer) return 0;

	if ( (flag == TIMER_replace_unconditionally) ||
		(flag == TIMER_replace_later_timer && expire < timer->expire) ||
		(flag == TIMER_replace_earlier_timer && expire > timer->expire) ) {
		// TODO - owner and data need being modified too
		do_mod_expire_time(timer, expire);
	}
	return timer->id;
}

// add a timer event
timer_id_t add_timer_event(list_head_t* l, timer_cb_func_t func, void* owner, void* data, time_t expire)
{
	static timer_id_t baseid = 0;

	timer_struct_t* timer = g_slice_alloc(sizeof *timer);

	INIT_LIST_HEAD(&timer->sprite_list);
	INIT_LIST_HEAD(&timer->entry);
	timer->function = func;
	timer->expire   = expire;
	timer->owner    = owner;
	timer->data     = data;
	timer->id       = ++baseid;
	if (timer->id == 0) timer->id = ++baseid;

	list_add_tail(&timer->sprite_list, l);
	add_timer(timer);

	return timer->id;
}

void mod_expire_time(list_head_t* head, timer_id_t tid, time_t exptm)
{
	timer_struct_t* t;
	list_head_t*    cur;

	list_for_each (cur, head) {
		t = list_entry (cur, timer_struct_t, sprite_list);
		if (t->id == tid) {
			do_mod_expire_time(t, exptm);
			break;
		}
	}
}

void remove_timer(list_head_t* head, timer_id_t tid)
{
	timer_struct_t *t;
	list_head_t    *cur;

	list_for_each (cur, head) {
		t = list_entry (cur, timer_struct_t, sprite_list);
		if (t->id == tid) {
			do_remove_timer(t, 0);
			break;
		}
	}
}

void remove_timers(list_head_t* head)
{
	timer_struct_t *t;
	list_head_t *l, *m;

	list_for_each_safe (l, m, head) {
		t = list_entry (l, timer_struct_t, sprite_list);
		do_remove_timer(t, 0);
	}
}

static void
scan_timer_list(const int idx)
{
	int    i;
	time_t min_exptm = 0;
	timer_struct_t *t;
	list_head_t *cur, *next;

	list_for_each_safe(cur, next, &vec[idx].head) {
		t = list_entry(cur, timer_struct_t, entry);
		if (t->function) {
			i = find_min_idx(t->expire - now.tv_sec, idx);
			if (i != idx) {
				list_del(&t->entry);
				list_add_tail(&t->entry, &vec[i].head);
				set_min_exptm(t->expire, i);
			} else if ((t->expire < min_exptm) || !min_exptm) {
				min_exptm = t->expire;
			}
		} else {
			do_remove_timer(t, 1);
		}
 	}

	vec[idx].min_expiring_time = min_exptm;
}

void scan_seconds_timer ()
{
	list_head_t *l, *p;
	timer_struct_t *t;

	list_for_each_safe (l, p, &vec[0].head) {
		t = list_entry (l, timer_struct_t, entry);
		if (!(t->function)) {
			do_remove_timer(t, 1);
		} else if (t->expire <= now.tv_sec) {
			if ( t->function(t->owner, t->data) == 0 ) {
				do_remove_timer(t, 1);
			}
		}
	}

	int i = 1;
	for (; i != TIMER_VEC_SIZE; ++i) {
		if ((vec[i].min_expiring_time - now.tv_sec) < 2) {
			scan_timer_list(i);
		}
	}
}

void destroy_timer ()
{
	int i;
	list_head_t *l, *p;

	for (i = 0; i < TIMER_VEC_SIZE; i++) {
		list_for_each_safe (l, p, &vec[i].head) {
			timer_struct_t *t = list_entry (l, timer_struct_t, entry);
			do_remove_timer (t, 1);
		}
	}

	list_for_each_safe (l, p, &micro_timer) {
		micro_timer_struct_t *t = list_entry (l, micro_timer_struct_t, entry);
		remove_micro_timer (t);
	}
}

micro_timer_struct_t *
add_micro_event (void (*function)(void *), struct timeval *tv, void * data)
{
	micro_timer_struct_t* timer = g_slice_alloc(sizeof *timer);
	INIT_LIST_HEAD (&timer->entry);
	timer->function = function;
	timer->tv = *tv;
	timer->arg = data;

	list_add_tail (&timer->entry, &micro_timer);
	return timer;
}

void scan_microseconds_timer ()
{
	list_head_t *l, *p;
	micro_timer_struct_t *t;

	list_for_each_safe (l, p, &micro_timer) {
		t = list_entry (l, micro_timer_struct_t, entry);
		if (now.tv_sec > t->tv.tv_sec || (now.tv_sec == t->tv.tv_sec && now.tv_usec > t->tv.tv_usec)) {
			t->function (t->arg);
			remove_micro_timer (t);
		}
	}
}

/*------------------------------------------
  *  inline function definitions
  *-----------------------------------------*/
static inline void
add_timer (timer_struct_t *t)
{
	int i, diff;

	diff = t->expire - now.tv_sec;
	for (i = 0; i != (TIMER_VEC_SIZE - 1); i++) {
		if (diff <= vec[i].expire)
			break;
	}

	list_add_tail(&t->entry, &vec[i].head);
	set_min_exptm(t->expire, i);
}

static inline void
do_remove_timer(timer_struct_t* t, int freed)
{
	if (t->sprite_list.next != 0) {
		list_del(&t->sprite_list);
	}
	if (freed) {
		list_del(&t->entry);
		g_slice_free1(sizeof *t, t);
	} else {
		t->function = 0;
	}
}

static inline void
remove_micro_timer (micro_timer_struct_t *t)
{
	list_del(&t->entry);
	g_slice_free1(sizeof *t, t);
}

static inline void
do_mod_expire_time(timer_struct_t* t, time_t expiretime)
{
	time_t diff = expiretime - now.tv_sec;
	t->expire   = expiretime;

	int i = 0;
	for ( ; i != (TIMER_VEC_SIZE - 1); ++i ) {
		if ( diff <= vec[i].expire ) break;
	}

	list_del_init(&t->entry);
	list_add_tail(&t->entry, &vec[i].head);
	set_min_exptm(t->expire, i);
}

static inline int
find_min_idx(time_t diff, int max_idx)
{
	while (max_idx && (vec[max_idx - 1].expire >= diff)) {
		--max_idx;
	}
	return max_idx;
}

static inline void
set_min_exptm(time_t exptm, int idx)
{
	if ((exptm < vec[idx].min_expiring_time) || (vec[idx].min_expiring_time == 0)) {
		vec[idx].min_expiring_time = exptm;
	}
}

static inline timer_struct_t*
find_timer(list_head_t* l, timer_cb_func_t func)
{
	if ( l->next != l ) {
		timer_struct_t *t;

		list_for_each_entry (t, l, sprite_list) {
			if (t->function == func)
				return t;
		}
	}
	return NULL;
}

static inline timer_struct_t*
find_timer_expires_at(list_head_t* l, timer_cb_func_t func, time_t expire)
{
	if ( l->next != l ) {
		timer_struct_t *t;

		list_for_each_entry (t, l, sprite_list) {
			if (t->function == func && t->expire == expire)
				return t;
		}
	}
	return NULL;
}
