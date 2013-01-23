/**
 * @file c_barrier_ring_queue.h
 * @brief 带屏障功能的环形缓冲实现
 * @author baron baron@taomee.com
 * @version 1.0
 * @date 2010-08-25
 */
#ifndef __C_BARRIER_RING_QUEUE_H__
#define __C_BARRIER_RING_QUEUE_H__


#include "i_barrier_ring_queue.h"

#include "list.h"
#include "rbtree.h"
#include <stdio.h>

#include <sys/types.h>

#define ERROR_ALREADY_INIT          -2
#define ERROR_NOT_INIT              -3
#define ERROR_INVALID_PARAM         -4
#define ERROR_MMAP_FAILED           -5
#define ERROR_NO_ENOUGH_MEM         -6
#define ERROR_NO_ENOUGH_RQ_BUFFER   -7
#define ERROR_NO_ENOUGH_BUFFER      -8
#define ERROR_ASSERT_ERROR          -9
#define ERROR_INIT_PIPE             -10

#define AFUTEX_RETURN(x)    do {afutex_unlock(&m_p_meta->futex); return x;} while (0)
#define ERROR_RETURN(x)     do {set_errno(x); return -1;} while (0)

struct barrier_ring_queue_meta {
    int futex;                  /**< futex锁变量 */
    int32_t total_length;       /**< 总长度 */
    struct assist_array_node *p_reserve_start; /**< 后半部可用区首地址 */
    struct {
        char *p_buffer;         /**< 环形缓冲区首地址 */
        int32_t buffer_len;     /**< 环形缓冲区大小 */
        int32_t head_index;     /**< 环形缓冲区头指针 */
        int32_t tail_index;     /**< 环形缓冲区尾指针 */
        int last_error;         /**< 上一次的出错码 */
    } ring_queue;               /**< 环形队列信息结构体 */
    struct {
        struct assist_array_node *p_head;       /**< 指向used list的头指针 */
        struct assist_array_node *p_read_head;  /**< 指向读指针 */
    } used_list;
    struct {
        struct assist_array_node *p_head;   /**< 指向empty list的首部 */
        int32_t reserve_head_index;         /**< 指向备用区域的首部 */
        int32_t reserve_tail_index;         /**< 指向备用区尾的首部 */
    } empty_list;
    struct {
        struct rb_root rb_root;             /**< 红黑数头结点 */
    } rb_tree;
};

struct assist_array_node {
    struct rb_node rb_node;
    int32_t key;
    struct list_head empty_used_list;
    struct list_head homo_list;
    int32_t addr;
    int32_t length;
    int32_t flag;   /**< 标识是否过滤以及进程ID */
}__attribute__((aligned(sizeof(long))));

/**
 * @brief 带屏障环形缓冲实现
 */
class c_barrier_ring_queue : public i_barrier_ring_queue
{
public:
    /**
     * @brief 构造函数
     */
    c_barrier_ring_queue();

    /**
     * @brief 析构函数
     */
    virtual ~c_barrier_ring_queue();

    /**
     * @brief 初始化函数
     *
     * @param rq_length         环形队列长度
     * @param avg_buf_length    环形队列接收数据buf的平均长度
     *
     * @return 0成功 -1失败
     */
    int init(int32_t rq_length, int32_t avg_buf_length);

    /**
     * @brief 反初始化函数
     *
     * @return 0成功 -1失败
     */
    int uninit();

    /**
     * @brief 把数据压入到环状缓冲区中
     *
     * @param p_data    指向要压入的数据
     * @param length    压入的数据的长度
     * @param key       屏障判断的关键字
     * @param flag      是否进行屏障的标志位
     *
     * @return 0成功 -1失败
     */
    int push(const char *p_data, int32_t length, int32_t key, int32_t flag);

    /**
     * @brief 把数据从环形缓冲区取出
     *
     * @param p_data    指向传出buf的缓冲区
     * @param data_len  缓冲区长度,用于内部判断是否会发生缓冲区溢出
     *
     * @return 0成功 -1失败
     */
    int pop(char *p_data, int32_t data_len);

    /**
     * @brief 为指定进程ID清理相应的数据包,以及信息结构.当进程异常退出时,或者处理超时时,由外部进程调用此接口进行清理
     *
     * @param pid   进程ID
     */
    void clean_4pid(int32_t pid);

    /**
     * @brief 为指定关键字进行清理数据包,以及信息结构.当进程异常退出时,或者超时时,由外部进程调用此接口进行清理
     *
     * @param key   关键字
     */
    void clean_4key(int32_t key);

    /**
     * @brief 获取上一次出错操作的错误码
     *
     * @return 整型错误码
     */
    int get_last_errno();

    /**
     * @brief 获取上一次出错操作的错误描述
     *
     * @return 错误描述
     */
    const char *get_last_errstr();

    /**
     * @brief 判断empty队列是否为空
     *
     * @return 0非空 1空
     */
    int empty_list_empty();

    /**
     * @brief 判断环形缓冲区是否已满
     *
     * @return 0非满 1满
     */
    int ring_queue_full();

    /**
     * @brief 当前状况时候可以pop
     *
     * @return 0可以pop 1没有可pop的数据
     */
    int is_able_pop();

    void get_debug_info(debug_info_t *p_debug_info);

private:
    /**
     * @brief 整个树结构信息头
     */
    struct barrier_ring_queue_meta *m_p_meta;

    /**
     * @brief 是否已初始化
     */
    int m_init;

    /**
     * @brief 上一次操作的特征值
     */
    int32_t m_last_key;

    /**
     * @brief 使用select时,管道的写描述符
     */
    int m_write_fd;

    /**
     * @brief 当前进程的PID
     */
    int32_t pid;

    /**
     * @brief 上一次处理的节点的地址
     */
    struct assist_array_node *m_p_assist_array_node;

    /**
     * @brief 从empty队列中pop出一个节点
     *
     * @return NULL失败 否则成功
     */
    struct assist_array_node *pop_empty_list_node();

    /**
     * @brief 删除used list的节点,会影响used list头部的移动
     *
     * @param p_assist_array_node  要从used队列中删除的节点
     */
    void used_list_del(struct assist_array_node *p_assist_array_node);

    /**
     * @brief 将节点压入empty队列
     *
     * @param p_used_list_node  要压入empty队列中的节点
     */
    void push_empty_list_node(struct assist_array_node *p_used_list_node);

    /**
     * @brief 从used队列中弹出一个节点
     *
     * @param pp_assist_array_node  传出参数,用来存放从used队列中弹出的节点
     * @param data_len  弹出的节点中buf长度信息不允许超过此值
     *
     * @return 0成功 -1失败
     */
    int pop_used_list_node(struct assist_array_node **pp_assist_array_node, int32_t data_len);

    /**
     * @brief 将节点压入used队列中
     *
     * @param p_empty_list_node 要压入used队列的节点
     */
    void push_used_list_node(struct assist_array_node *p_empty_list_node);

    /**
     * @brief 红黑树插入
     *
     * @param p_rb_root 红黑树树根
     * @param key       红黑树键值
     * @param p_rb_node 要插入的节点
     *
     * @return NULL 红黑数节点地址
     */
    struct assist_array_node *rb_insert(struct rb_root *p_rb_root, int32_t key, struct rb_node *p_rb_node);

    /**
     * @brief 红黑树插入
     *
     * @param p_rb_root 红黑树树根
     * @param key       红黑树键值
     * @param p_rb_node 要插入的节点
     *
     * @return NULL 红黑数节点地址
     */
    struct assist_array_node *__rb_insert(struct rb_root *p_rb_root, int32_t key, struct rb_node *p_rb_node);

    /**
     * @brief 红黑树搜索
     *
     * @param p_rb_root 红黑树树根
     * @param key       红黑树键值
     *
     * @return NULL未查到 否则为查询导的节点的地址
     */
    struct assist_array_node *rb_search(struct rb_root *p_rb_root, int32_t key);

    /**
     * @brief 根据进程ID搜索红黑树
     *
     * @param p_rb_node 红黑数根节点
     * @param pid       进程ID
     *
     * @return NULL未查到 否则为查询导的节点的地址
     */
    struct assist_array_node *rb_search_4pid(struct rb_node *p_rb_node, pid_t pid);

    /**
     * @brief 移动环形队列头指针(当used队列头结点发生变化是进行调用)
     */
    void move_ring_queue_head_index();

protected:
    int m_last_errno;

    /**
     * @brief 设置操作错误码
     *
     * @param _errno 整型错误码
     */
    void set_errno(int _errno);

    /**
     * @brief 获取当前进程ID
     *
     * @return 进程ID
     */
    int32_t get_pid();
};

inline void c_barrier_ring_queue::used_list_del(struct assist_array_node *p_assist_array_node)
{
   if (m_p_meta->used_list.p_head == p_assist_array_node)
   {
       if (list_empty(&p_assist_array_node->empty_used_list))
       {
            // 若要摘取used list最后一个节点
            m_p_meta->used_list.p_head = NULL;
            move_ring_queue_head_index();
            return;
       }

       // pop出used list头结点
       m_p_meta->used_list.p_head = list_entry(p_assist_array_node->empty_used_list.next, struct assist_array_node, empty_used_list);
       move_ring_queue_head_index();
   }

   list_del(&p_assist_array_node->empty_used_list);

   return;
}

inline struct assist_array_node *c_barrier_ring_queue::pop_empty_list_node()
{
    if (!m_p_meta->empty_list.p_head)
    {
        // 空闲队列为空,则从后备内存区取
        if (m_p_meta->empty_list.reserve_tail_index - m_p_meta->empty_list.reserve_head_index <= 0)
        {
            // 备用区域已空
            return NULL;
        }

        return reinterpret_cast<struct assist_array_node *>(m_p_meta->p_reserve_start + m_p_meta->empty_list.reserve_head_index ++);
    }

    struct assist_array_node *p_empty_list_node = m_p_meta->empty_list.p_head;

    if (list_is_last(&p_empty_list_node->empty_used_list, &p_empty_list_node->empty_used_list))
    {
        // 空闲队列最后的节点
        m_p_meta->empty_list.p_head = NULL;
    }
    else
    {
        // pop出队头节点
        m_p_meta->empty_list.p_head = list_entry(p_empty_list_node->empty_used_list.next, struct assist_array_node, empty_used_list);
        list_del(&p_empty_list_node->empty_used_list);
    }

    return p_empty_list_node;
}

inline void c_barrier_ring_queue::push_empty_list_node(struct assist_array_node *p_used_list_node)
{
    // 当前空节点链表不空,则插入队尾,否则直接作首节点
    if (m_p_meta->empty_list.p_head)
    {
        list_add_tail(&p_used_list_node->empty_used_list, &m_p_meta->empty_list.p_head->empty_used_list);
        return;
    }

    INIT_LIST_HEAD(&p_used_list_node->empty_used_list);
    m_p_meta->empty_list.p_head = p_used_list_node;

    return;
}

inline void c_barrier_ring_queue::push_used_list_node(struct assist_array_node *p_empty_list_node)
{
    if (m_p_meta->used_list.p_head)
    {
        list_add_tail(&p_empty_list_node->empty_used_list, &m_p_meta->used_list.p_head->empty_used_list);

        if (!m_p_meta->used_list.p_read_head)
        {
            m_p_meta->used_list.p_read_head = p_empty_list_node;
        }

        return;
    }

    INIT_LIST_HEAD(&p_empty_list_node->empty_used_list);
    m_p_meta->used_list.p_head = p_empty_list_node;
    m_p_meta->used_list.p_read_head = p_empty_list_node;

    return;
}

inline int c_barrier_ring_queue::pop_used_list_node(struct assist_array_node **pp_assist_array_node, int32_t data_len)
{
    // 获取红黑树树根
    struct rb_root *p_rb_root = &m_p_meta->rb_tree.rb_root;
    struct assist_array_node *p_assist_array_node = NULL;

    if (m_p_assist_array_node)
    { // 上次是正常从红黑数的homo list上摘取节点
        struct list_head *p_homo_list_node = &m_p_assist_array_node->homo_list;

        if (!list_empty(p_homo_list_node))
        {
            // 将homo list的下一个节点设为红黑树节点,并将原homo list头节点从链表中删除
            p_assist_array_node = list_entry(p_homo_list_node->next, struct assist_array_node, homo_list);

            if (p_assist_array_node->length > data_len)
            {
                // 缓冲区长度不够,不允许pop,此时不改变之前的任何环境参数
                ERROR_RETURN(ERROR_NO_ENOUGH_BUFFER);
            }

            rb_replace_node(&m_p_assist_array_node->rb_node, &p_assist_array_node->rb_node, p_rb_root);
            list_del(p_homo_list_node);

            // 摘取used list节点,若p_head所指向的节点被移至empty list,则p_head往后移动一个节点(!关系到ring queue指针的移动)
            used_list_del(m_p_assist_array_node);
            // 将pop的used list节点push至empty list中
            push_empty_list_node(m_p_assist_array_node);

            *pp_assist_array_node = m_p_assist_array_node = p_assist_array_node;
            m_p_assist_array_node->flag = get_pid();
            return 0;
        }
    }
    else
    {
        p_assist_array_node = rb_search_4pid(p_rb_root->rb_node, 0);

        if (p_assist_array_node)
        {
            *pp_assist_array_node = m_p_assist_array_node = p_assist_array_node;
            m_p_assist_array_node->flag = get_pid();
            m_last_key = p_assist_array_node->key;
            return 0;
        }
    }

loop:
    // 读取used list节点,并判断是否为空
    if (!(p_assist_array_node = m_p_meta->used_list.p_read_head))
    {
        // 当前没有可以读取的节点
        *pp_assist_array_node = NULL;
        return 0;
    }

    if (p_assist_array_node->length > data_len)
    {
        ERROR_RETURN(ERROR_NO_ENOUGH_BUFFER);
    }

    // 移动used list读指针
    if (list_is_last(&p_assist_array_node->empty_used_list, &m_p_meta->used_list.p_head->empty_used_list))
    {
        // 获取最后一个used节点,并置读指针NULL
        m_p_meta->used_list.p_read_head = NULL;
    }
    else
    {
        // 读指针向后移动
        m_p_meta->used_list.p_read_head = list_entry(p_assist_array_node->empty_used_list.next, struct assist_array_node, empty_used_list);
    }

    struct assist_array_node *p_tmp_assist_array_node = NULL;

    if (m_p_assist_array_node)
    {
        if (m_last_key != p_assist_array_node->key)
        {
            if ((p_tmp_assist_array_node = rb_insert(p_rb_root, p_assist_array_node->key, &p_assist_array_node->rb_node)))
            {
                // 发生冲突
                list_add_tail(&p_assist_array_node->homo_list, &p_tmp_assist_array_node->homo_list);
            }
            else
            {
                INIT_LIST_HEAD(&p_assist_array_node->homo_list);
                p_assist_array_node->flag = 0;
            }

            goto loop;
        }

        rb_replace_node(&m_p_assist_array_node->rb_node, &p_assist_array_node->rb_node, p_rb_root);
        used_list_del(m_p_assist_array_node);
        push_empty_list_node(m_p_assist_array_node);
    }
    else
    {
        if ((p_tmp_assist_array_node = rb_insert(p_rb_root, p_assist_array_node->key, &p_assist_array_node->rb_node)))
        {
            // 发生冲突
            list_add_tail(&p_assist_array_node->homo_list, &p_tmp_assist_array_node->homo_list);
            goto loop;
        }

        m_last_key = p_assist_array_node->key;
    }

    *pp_assist_array_node = m_p_assist_array_node = p_assist_array_node;
    INIT_LIST_HEAD(&p_assist_array_node->homo_list);
    m_p_assist_array_node->flag = get_pid();
    return 0;
}

inline struct assist_array_node *c_barrier_ring_queue::rb_search(struct rb_root *p_rb_root, int32_t key)
{
    struct rb_node *n = p_rb_root->rb_node;
    struct assist_array_node *p_assist_array_node;

    while (n)
    {
        p_assist_array_node = rb_entry(n, struct assist_array_node, rb_node);

        if (key < p_assist_array_node->key)
        {
            n = n->rb_left;
        }
        else if (key > p_assist_array_node->key)
        {
            n = n->rb_right;
        }
        else
        {
            return p_assist_array_node;
        }
    }

    return NULL;
}

inline struct assist_array_node *c_barrier_ring_queue::rb_search_4pid(struct rb_node *p_rb_node, int32_t pid)
{
    if (!p_rb_node)
    {
        return NULL;
    }

    struct assist_array_node *p_assist_array_node = rb_entry(p_rb_node, struct assist_array_node, rb_node);

    if (p_assist_array_node->flag == pid)
    {
        return p_assist_array_node;
    }

    if ((p_assist_array_node = rb_search_4pid(p_rb_node->rb_right, pid)))
    {
        return p_assist_array_node;
    }

    return rb_search_4pid(p_rb_node->rb_left, pid);
}

inline struct assist_array_node *c_barrier_ring_queue::__rb_insert(struct rb_root *p_rb_root, int32_t key, struct rb_node *p_rb_node)
{
    struct rb_node **p = &p_rb_root->rb_node;
    struct rb_node *parent = NULL;
    struct assist_array_node *p_assist_array_node;

    while (*p)
    {
        parent = *p;
        p_assist_array_node = rb_entry(parent, struct assist_array_node, rb_node);

        if (key < p_assist_array_node->key)
        {
            p = &(*p)->rb_left;
        }
        else if(key > p_assist_array_node->key)
        {
            p = &(*p)->rb_right;
        }
        else
        {
            return p_assist_array_node;
        }
    }

    rb_link_node(p_rb_node, parent, p);

    return NULL;
}

inline struct assist_array_node *c_barrier_ring_queue::rb_insert(struct rb_root *p_rb_root, int32_t key, struct rb_node *p_rb_node)
{
    struct assist_array_node *p_assist_array_node = NULL;

    if (!(p_assist_array_node = __rb_insert(p_rb_root, key, p_rb_node)))
    {
        rb_insert_color(p_rb_node, p_rb_root);
    }

    return p_assist_array_node;
}

inline int c_barrier_ring_queue::empty_list_empty()
{
    return !(m_p_meta->empty_list.p_head || m_p_meta->empty_list.reserve_head_index < m_p_meta->empty_list.reserve_tail_index);
}

inline int c_barrier_ring_queue::ring_queue_full()
{
    return
        1 == ((m_p_meta->ring_queue.tail_index >= m_p_meta->ring_queue.head_index) ?
                (m_p_meta->ring_queue.buffer_len - m_p_meta->ring_queue.tail_index + m_p_meta->ring_queue.head_index)
                : (m_p_meta->ring_queue.head_index - m_p_meta->ring_queue.tail_index));
}

inline int c_barrier_ring_queue::is_able_pop()
{
    // 有可读数据或者或者相关的homo队列不空
    return m_p_meta->used_list.p_read_head
            || !m_p_assist_array_node
            || !list_empty(&m_p_assist_array_node->homo_list);
}

inline void c_barrier_ring_queue::move_ring_queue_head_index()
{
    // 获得used list节点,调整环形缓冲区头部index位置
    if (m_p_meta->used_list.p_head)
    {
        m_p_meta->ring_queue.head_index = m_p_meta->used_list.p_head->addr;
    }
    else
    {
        // 当前缓冲区空
        m_p_meta->ring_queue.head_index = m_p_meta->ring_queue.tail_index;
    }

    return;
}

inline void c_barrier_ring_queue::set_errno(int _errno)
{
    m_last_errno = _errno;
    return;
}

inline int c_barrier_ring_queue::get_last_errno()
{
    return m_last_errno;
}

inline int32_t c_barrier_ring_queue::get_pid()
{
    if (!pid)
    {
        pid = static_cast<int>(getpid());
    }

    return pid;
}

#endif //!__C_BARRIER_RING_QUEUE_H__
