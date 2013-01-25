#ifndef Q_LIST_H
#define Q_LIST_H

#include "internal.h"
#include "list.h"

typedef struct _qlist {
	struct _Q_MUTEX 	qmutex;
	list_head_t  		head;
}qlist_t;

#define q_enter_list_for_each_safe(pos, n, ql) \
	Q_MUTEX_ENTER(ql.qmutex); \
	list_head_t* head = &(ql.head); \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

#define q_exit_list_for_each_safe(ql) \
		Q_MUTEX_LEAVE(ql.qmutex);

#define qlist_init(ql) \
		Q_MUTEX_INIT(ql.qmutex, 1); \
		INIT_LIST_HEAD(&ql.head)

#define qlist_add_tail(ql, new_entry, head) \
		Q_MUTEX_ENTER(ql.qmutex); \
		list_add_tail(new_entry, head); \
		Q_MUTEX_LEAVE(ql.qmutex)

#define qlist_add(ql, new_entry, head) \
		Q_MUTEX_ENTER(ql.qmutex); \
		list_add(new_entry, head); \
		Q_MUTEX_LEAVE(ql.qmutex)


#endif
