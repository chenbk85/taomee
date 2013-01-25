/**
 *============================================================
 *  @file      list.h
 *  @brief     从内核代码里抽取出来的侵入式链表操作函数/宏。\n
 *             Simple doubly linked list implementation.\n
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef TAOMEE_H_LIST_H_
#define TAOMEE_H_LIST_H_

#define __builtin_prefetch(x,y,z) 1

/**
 * @brief 链表头结点类型，同时也用来连接每个子结点。
 */
struct list_head
{
  struct list_head *next, *prev;
};
/**
 * @brief a typedef
 */
typedef struct list_head list_head_t;

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

/**
 * @brief 初始化头节点。初始化后，ptr->next和ptr->prev都指向ptr本身。
 * @param ptr 头节点指针
 */
#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/*
 * Insert a new entry between two known consecutive entries. 
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void
__list_add (struct list_head *new_entry,
	    struct list_head *prev, struct list_head *next)
{
  next->prev = new_entry;
  new_entry->next = next;
  new_entry->prev = prev;
  prev->next = new_entry;
}

/**
 * @brief add a new entry\n
 *        Insert a new entry after the specified head.
 *        This is good for implementing stacks.
 * @param new_entry new entry to be added
 * @param head list head to add it after
 */
static inline void
list_add (struct list_head *new_entry, struct list_head *head)
{
  __list_add (new_entry, head, head->next);
}

/**
 * @brief add a new entry\n
 *        Insert a new entry before the specified head.
 *        This is useful for implementing queues.
 * @param new_entry new entry to be added
 * @param head list head to add it before
 */
static inline void
list_add_tail (struct list_head *new_entry, struct list_head *head)
{
  __list_add (new_entry, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void
__list_del (struct list_head *prev, struct list_head *next)
{
  next->prev = prev;
  prev->next = next;
}

/**
 * @brief deletes entry from list.\n
 * @param entry the element to delete from the list.
 * @note  list_empty on entry does not return true after this, the entry is in an undefined state.
 */
static inline void
list_del (struct list_head *entry)
{
  __list_del (entry->prev, entry->next);
  entry->next = 0;
  entry->prev = 0;
}

/**
 * @brief deletes entry from list and reinitialize it.
 * @param entry the element to delete from the list.
 */
static inline void
list_del_init (struct list_head *entry)
{
  __list_del (entry->prev, entry->next);
  INIT_LIST_HEAD (entry);
}

/**
 * @brief delete from one list and add as another's head
 * @param list the entry to move
 * @param head the head that will precede our entry
 */
static inline void
list_move (struct list_head *list, struct list_head *head)
{
  __list_del (list->prev, list->next);
  list_add (list, head);
}

/**
 * @brief delete from one list and add as another's tail
 * @param list the entry to move
 * @param head the head that will follow our entry
 */
static inline void
list_move_tail (struct list_head *list, struct list_head *head)
{
  __list_del (list->prev, list->next);
  list_add_tail (list, head);
}

/**
 * @brief tests whether a list is empty
 * @param head the list to test.
 */
static inline int
list_empty (struct list_head *head)
{
  return head->next == head;
}

static inline void
__list_splice (struct list_head *list, struct list_head *head)
{
  struct list_head *first = list->next;
  struct list_head *last = list->prev;
  struct list_head *at = head->next;

  first->prev = head;
  head->next = first;

  last->next = at;
  at->prev = last;
}

/**
 * @brief join two lists
 * @param list the new list to add.
 * @param head the place to add it in the first list.
 */
static inline void
list_splice (struct list_head *list, struct list_head *head)
{
  if (!list_empty (list))
    __list_splice (list, head);
}

/**
 * @brief join two lists and reinitialise the emptied list.
 * @param list the new list to add.
 * @param head the place to add it in the first list.
 *
 * @note 'list' is reinitialised
 */
static inline void
list_splice_init (struct list_head *list, struct list_head *head)
{
  if (!list_empty (list))
    {
      __list_splice (list, head);
      INIT_LIST_HEAD (list);
    }
}

/**
 * @brief get the struct for this entry
 * @param ptr the &struct list_head pointer.
 * @param type the type of the struct this is embedded in.
 * @param member the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
 * @brief iterate over a list
 * @param pos the &struct list_head to use as a loop counter.
 * @param head the head for your list.
 */
#ifdef  __cplusplus
#define list_for_each(pos, head) \
		__list_for_each(pos, head)
#else
#define list_for_each(pos, head) \
	    for (pos = (head)->next, __builtin_prefetch(pos->next,0,1); \
			 pos != (head); \
			 pos = pos->next, __builtin_prefetch(pos->next,0,1))
#endif

#define __list_for_each(pos, head) \
	    for (pos = (head)->next; pos != (head); pos = pos->next)
/**
 * @brief iterate over a list backwards
 * @param pos the &struct list_head to use as a loop counter.
 * @param head the head for your list.
 */
#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev, __builtin_prefetch(pos->prev,0,1); \
	 pos != (head); \
	 pos = pos->prev, __builtin_prefetch(pos->prev,0,1))

#define __list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); \
        	pos = pos->prev)

/**
 * @brief iterate over a list safe against removal of list entry
 * @param pos the &struct list_head to use as a loop counter.
 * @param n nother &struct list_head to use as temporary storage
 * @param head the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * @brief iterate over list of given type
 * @param pos the type * to use as a loop counter.
 * @param head the head for your list.
 * @param member the name of the list_struct within the struct.
 */
#ifdef  __cplusplus
#define list_for_each_entry(pos, head, member)				\
		for (pos = list_entry((head)->next, typeof(*pos), member);	\
		     &pos->member != (head); 					\
		     pos = list_entry(pos->member.next, typeof(*pos), member))
#else
#define list_for_each_entry(pos, head, member)				\
		for (pos = list_entry((head)->next, typeof(*pos), member),	\
		     __builtin_prefetch(pos->member.next,0,1);			\
		     &pos->member != (head); 					\
		     pos = list_entry(pos->member.next, typeof(*pos), member),	\
		     __builtin_prefetch(pos->member.next,0,1))
#endif

#endif // TAOMEE_H_LIST_H_
