/**
 * @file c_barrier_ring_queue.cpp
 * @brief 带屏障功能的环形缓冲实现
 * @author baron baron@taomee.com
 * @version 1.0
 * @date 2010-08-25
 */
#include <stdlib.h>
#include <string.h>
#include <new>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

#include "c_barrier_ring_queue.h"
#include "afutex.h"

int create_barrier_ring_queue_instance(i_barrier_ring_queue **pp_instance)
{
    if (NULL == pp_instance)
    {
        return -1;
    }

    c_barrier_ring_queue *p_instance = new (std::nothrow) c_barrier_ring_queue();

    if (NULL == p_instance)
    {
        return -1;
    }
    else
    {
        *pp_instance = dynamic_cast<i_barrier_ring_queue *>(p_instance);
        return 0;
    }
}

c_barrier_ring_queue::c_barrier_ring_queue()
    :m_p_meta(NULL),
     m_init(0),
     m_last_key(-1),
     pid(0),
     m_p_assist_array_node(NULL),
     m_last_errno(0)
{
}

c_barrier_ring_queue::~c_barrier_ring_queue()
{
}

int c_barrier_ring_queue::init(int32_t rq_length, int32_t avg_buf_length)
{
    if (m_init)
    {
        ERROR_RETURN(ERROR_ALREADY_INIT);
    }

    if (rq_length <= 0 || 0 == avg_buf_length)
    {
        ERROR_RETURN(ERROR_INVALID_PARAM);
    }

    int32_t assist_array_length = (rq_length / avg_buf_length + 1) * sizeof(struct assist_array_node);
    ++ rq_length;

    // 总长度是头部信息结构体+环形队列+1+数据包信息array区域长度
    int32_t total_length = static_cast<int32_t>(sizeof(barrier_ring_queue_meta))  + rq_length + assist_array_length;
    char *p_shm = reinterpret_cast<char *>(mmap(NULL, total_length, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0));

    if (MAP_FAILED == p_shm)
    {
        ERROR_RETURN(ERROR_MMAP_FAILED);
    }

    m_p_meta = reinterpret_cast<struct barrier_ring_queue_meta *>(p_shm);

    // 为成员变量赋值及初始化
    afutex_mutex_init(&m_p_meta->futex);    /**< 初始化futex锁 */
    m_p_meta->total_length = total_length;  /**< 初始化总长度 */
    unsigned long tmp = reinterpret_cast<unsigned long>(p_shm + sizeof(struct barrier_ring_queue_meta) + rq_length);
    // 后部保留区于必须靠long字节大小对齐
    m_p_meta->p_reserve_start = reinterpret_cast<struct assist_array_node *>(tmp % sizeof(long) ? (tmp + sizeof(long) - tmp % sizeof(long)) : tmp);

    // 初始化环形队列
    m_p_meta->ring_queue.p_buffer = p_shm + sizeof(struct barrier_ring_queue_meta);
    m_p_meta->ring_queue.buffer_len = rq_length;
    m_p_meta->ring_queue.head_index = 0;
    m_p_meta->ring_queue.tail_index = 0;
    m_p_meta->ring_queue.last_error = 0;

    // 初始化used list和empty list的头指针
    m_p_meta->empty_list.p_head = NULL;
    m_p_meta->empty_list.reserve_head_index = 0;
    m_p_meta->empty_list.reserve_tail_index = static_cast<int32_t>((reinterpret_cast<unsigned long>(p_shm + m_p_meta->total_length) - reinterpret_cast<unsigned long>(m_p_meta->p_reserve_start)) / sizeof(struct assist_array_node));

    if (m_p_meta->empty_list.reserve_tail_index <= 0)
    {
        munmap(&m_p_meta, m_p_meta->total_length);
        ERROR_RETURN(ERROR_NO_ENOUGH_MEM);
    }

    m_p_meta->used_list.p_head = NULL;
    m_p_meta->used_list.p_read_head = NULL;

    // 初始化rb_tree的头指针
    m_p_meta->rb_tree.rb_root = RB_ROOT;
    m_init = 1;

    return 0;
}

int c_barrier_ring_queue::uninit()
{
    if (!m_init)
    {
        ERROR_RETURN(ERROR_NOT_INIT);
    }

    munmap(&m_p_meta, m_p_meta->total_length);
    m_p_meta = NULL;
    m_init = 0;

    return 0;
}

int c_barrier_ring_queue::push(const char *p_data, int32_t length, int32_t key, int32_t flag)
{
    if (!m_init)
    {
        ERROR_RETURN(ERROR_NOT_INIT);
    }

    if (NULL == p_data || length <= 0)
    {
        ERROR_RETURN(ERROR_INVALID_PARAM);
    }

    // 若有crash进程的数据包阻塞在最前则清理,一定程度的完成自清理工作
    while (empty_list_empty()) // || ring_queue_full()
    {
        if (m_p_meta->used_list.p_head == m_p_meta->used_list.p_read_head)
        {
            set_errno(ERROR_NO_ENOUGH_MEM);
            return -1;
        }

        int32_t pid = m_p_meta->used_list.p_head->flag;

        if (kill(static_cast<pid_t>(pid), 0) && ESRCH == errno)
        {
            clean_4pid(pid);
        }
        else
        {
            set_errno(ERROR_NO_ENOUGH_MEM);
            return -1;
        }
    }

    char *p_buffer = m_p_meta->ring_queue.p_buffer;
    int32_t head_index = m_p_meta->ring_queue.head_index;
    int32_t tail_index = m_p_meta->ring_queue.tail_index;
    int32_t buffer_len = m_p_meta->ring_queue.buffer_len;
    int32_t addr = -1;

    if (tail_index >= head_index)
    {
        // 获取当前环形缓冲区空闲区域大小
        int32_t cur_empty_buffer_len = buffer_len - (tail_index - head_index) - 1;

        if (length > cur_empty_buffer_len)
        {
            set_errno(ERROR_NO_ENOUGH_RQ_BUFFER);
            return -1;
        }

        int32_t partial_empty_buffer_len = buffer_len - tail_index;

        if (length < partial_empty_buffer_len)
        {
            mempcpy(p_buffer + tail_index, p_data, length);
            addr = m_p_meta->ring_queue.tail_index;
            m_p_meta->ring_queue.tail_index += length;
        }
        else
        {
            mempcpy(p_buffer + tail_index, p_data, partial_empty_buffer_len);
            mempcpy(p_buffer, p_data + partial_empty_buffer_len, length - partial_empty_buffer_len);
            addr = m_p_meta->ring_queue.tail_index;
            m_p_meta->ring_queue.tail_index = length - partial_empty_buffer_len;
        }
    }
    else
    {
        int32_t cur_empty_buffer_len = head_index - tail_index - 1;

        if (length > cur_empty_buffer_len)
        {
            set_errno(ERROR_NO_ENOUGH_RQ_BUFFER);
            return -1;
        }

        mempcpy(p_buffer + tail_index, p_data, length);
        addr = m_p_meta->ring_queue.tail_index;
        m_p_meta->ring_queue.tail_index += length;
    }

    struct assist_array_node *p_empty_list_node = pop_empty_list_node();

    p_empty_list_node->key = key;
    p_empty_list_node->flag = flag;
    p_empty_list_node->addr = addr;
    p_empty_list_node->length = length;
    push_used_list_node(p_empty_list_node);

    return 0;
}

int c_barrier_ring_queue::pop(char *p_data, int32_t data_len)
{
    if (!m_init)
    {
        ERROR_RETURN(ERROR_NOT_INIT);
    }

    if (NULL == p_data || data_len <= 0)
    {
        ERROR_RETURN(ERROR_INVALID_PARAM);
    }

    struct assist_array_node *p_assist_array_node = NULL;

    if (pop_used_list_node(&p_assist_array_node, data_len))
    {
        ERROR_RETURN(ERROR_INVALID_PARAM);
    }

    if (!p_assist_array_node)
    {
        return 0;
    }

    int32_t length = p_assist_array_node->length;
    int32_t addr = p_assist_array_node->addr;
    char *p_buffer = m_p_meta->ring_queue.p_buffer;

    if (p_assist_array_node->addr <= m_p_meta->ring_queue.tail_index)
    {
        mempcpy(p_data, p_buffer + addr, length);
    }
    else
    {
        int partial_buffer_len = m_p_meta->ring_queue.buffer_len - addr;

        if (length < partial_buffer_len)
        {
            mempcpy(p_data, p_buffer + addr, length);
        }
        else
        {
            mempcpy(p_data, p_buffer + addr, partial_buffer_len);
            mempcpy(p_data + partial_buffer_len, p_buffer, length - partial_buffer_len);
        }
    }

    return length;
}

void c_barrier_ring_queue::clean_4pid(int32_t pid)
{
    struct assist_array_node *p_assist_array_node = NULL;

    if (!(p_assist_array_node = rb_search_4pid(m_p_meta->rb_tree.rb_root.rb_node, pid)))
    {
        return;
    }

    rb_erase(&p_assist_array_node->rb_node, &m_p_meta->rb_tree.rb_root);

    struct assist_array_node *pos;

    list_for_each_entry(pos, &p_assist_array_node->homo_list, homo_list)
    {
        used_list_del(pos);
        push_empty_list_node(pos);
    }

    used_list_del(p_assist_array_node);
    push_empty_list_node(p_assist_array_node);

    return;
}

void c_barrier_ring_queue::clean_4key(int32_t key)
{
    struct assist_array_node *p_assist_array_node = NULL;

    if (!(p_assist_array_node = rb_search(&m_p_meta->rb_tree.rb_root, key)))
    {
        return;
    }

    rb_erase(&p_assist_array_node->rb_node, &m_p_meta->rb_tree.rb_root);

    struct assist_array_node *pos;

    list_for_each_entry(pos, &p_assist_array_node->homo_list, homo_list)
    {
        used_list_del(pos);
        push_empty_list_node(pos);
    }

    used_list_del(p_assist_array_node);
    push_empty_list_node(p_assist_array_node);

    return;
}

const char *c_barrier_ring_queue::get_last_errstr()
{
    switch (get_last_errno())
    {
        case 0:
            return "success";
        case ERROR_NOT_INIT:
            return "not init";
        case ERROR_INVALID_PARAM:
            return "invalid param";
        case ERROR_MMAP_FAILED:
            return "mmap failed";
        case ERROR_NO_ENOUGH_MEM:
            return "out of memory";
        case ERROR_NO_ENOUGH_RQ_BUFFER:
            return "out of rq buffer";
        case ERROR_NO_ENOUGH_BUFFER:
            return "out of buffer";
        case ERROR_ASSERT_ERROR:
            return "assert error";
        case ERROR_INIT_PIPE:
            return "pipe failed";
        default:
            return "unknown error";
    }
}

int tree_traverse(struct rb_node *p_node);

void c_barrier_ring_queue::get_debug_info(debug_info_t *p_debug_info)
{
    if (NULL == p_debug_info)
    {
        return;
    }

    int count = 0;

    struct assist_array_node *pos;

    if (NULL == m_p_meta->used_list.p_head)
    {
        p_debug_info->used_list_length = 0;
    }
    else
    {
        count = 1;

        list_for_each_entry(pos, &m_p_meta->used_list.p_head->empty_used_list, empty_used_list)
        {
            ++ count;
        }

        p_debug_info->used_list_length = count;
    }

    if (NULL == m_p_meta->empty_list.p_head)
    {
        p_debug_info->empty_list_length = 0;
    }
    else
    {
        count = 1;

        list_for_each_entry(pos, &m_p_meta->empty_list.p_head->empty_used_list, empty_used_list)
        {
            ++ count;
        }

        p_debug_info->empty_list_length = count;
    }

    p_debug_info->reserve_length = m_p_meta->empty_list.reserve_tail_index - m_p_meta->empty_list.reserve_head_index;

    if (!m_p_meta->used_list.p_head)
    {
        p_debug_info->conflict_cnt = p_debug_info->used_list_length;
    }
    else
    {
        count = 1;

        list_for_each_entry(pos, &m_p_meta->used_list.p_head->empty_used_list, empty_used_list)
        {
            if (pos == m_p_meta->used_list.p_read_head)
            {
                break;
            }

            ++ count;
        }

        p_debug_info->conflict_cnt = count;
    }

    p_debug_info->tree_node_cnt = tree_traverse(m_p_meta->rb_tree.rb_root.rb_node);

    return;
}

int tree_traverse(struct rb_node *p_node)
{
    if (!p_node)
    {
        return 0;
    }

    int total = tree_traverse(p_node->rb_left) + tree_traverse(p_node->rb_right) + 1;

    return total;
}
