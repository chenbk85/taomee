/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file net_io_server.h
 * @author baron <baron@taomee.com>
 * @author richard <richard@taomee.com>
 * @date 2011-04-06
 */

#ifndef NET_IO_SERVER_H_2011_07_19
#define NET_IO_SERVER_H_2011_07_19

#include "i_net_io_server.h"

#define NET_IO_SERVER_LISTEN_BACKLOG   80
#define MAX_CLIENT_NUM                 (20 * 1024)
#define MAX_ACTIVE_CLIENT_NUM          4096

#define MAX_ERRSTR_LEN                 1024

#define NET_IO_SERVER_FLAG_RUNOUT      0x0001

#include <stdio.h>
#include <stdarg.h>
#include <map>
#include <sys/epoll.h>

#include "list.h"

class net_io_server: public i_net_io_server
{
public:
    /**
     * @brief 构造函数
     */
    net_io_server();

    /**
     * @brief 析构函数
     */
    ~net_io_server();

    /**
     * @brief 对象初始化
     * @param ip  服务器监听IP地址
     * @param port  服务器监听端口地址
     * @param p_events_handler  服务器事件处理接口方法集合
     * @param p_events_notifier 服务器提醒机制处理对象
     * @param id_base  连接ID自增长基值
     * @param id_step  连接ID增加步长
     * @return 0-成功 -1-失败
     */
    int init(const char *ip, int port,
             const i_net_io_event_handler *p_events_handler,
             const i_net_io_notifier *p_net_io_notifier,
             int id_base = 1, int id_step = 1);

    /**
     * @brief 发送数据(尽可能多的发送数据或拷贝至发送缓冲区)
     * @param id  连接ID
     * @param p_data  欲发送的数据缓存
     * @param data_len  发送数据长度
     * @return >=0-处理数据的总长度 -1-失败
     */
    int send_data(int id, const char *p_data, int data_len);

    /**
     * @brief 广播,包括一个连接像全体连接广播,服务端像全体连接广播
     * @param id  连接ID
     * @param p_data  欲发送的数据缓存
     * @param date_len  发送数据长度
     * @return
     */
    int broadcast(int id, const char *p_data, int date_len, int is_atomic);

    /**
     * @brief 获取连接信息
     * @param id  连接ID
     * @param ip  连接IP
     * @param ip_buffer_len  存放IP缓冲区的长度
     * @param p_port  指向连接PORT
     * @param p_time  指向连接建立时间
     * @param p_fd  指向连接对应的文件描述符
     * @param pp_storage  指向连接对应的用户私有数据的指针
     * @return 0-成功 -1-失败
     */
    int get_connection_info(int id, char *ip, int ip_buffer_len, int *p_port,
                            int *p_time, int *p_fd, union net_io_storage **pp_storage);

    /**
     * @brief 枚举所有连接ID
     * @param p_buffer 存放连接ID
     * @param length  存放连接ID的缓冲区长度
     * @param p_buffer_len  返回的连接ID个数
     * @return 0-成功 -1-失败
     */
    int enum_connections(int *p_buffer, int length, int *p_buffer_len);

    /**
     * @brief 发送接收网络数据
     * @param timeout  超时限制
     * @param cmd  发送接收命令标志,包括:
     *             NET_IO_SERVER_CMD_ACCEPT,NET_IO_SERVER_CMD_READ,NET_IO_SERVER_CMD_WRITE
     * @return 0-成功 -1-失败
     */
    int do_io(int timeout, int cmd);

    /**
     * @brief 关闭连接
     * @param id  连接ID
     * @param need_notify  是否抛出事件
     * @return 0-成功 -1-失败
     */
    int close_connection(int id, bool need_notify);

    /**
     * @brief 获取上次出错的错误码
     * @return 错误码
     */
    int get_last_errno();

    /**
     * @brief 获取上次出错的错误信息
     * @return 错误信息
     */
    const char *get_last_errstr();

    /**
     * @brief 反初始化
     * @return 0-成功 -1-失败
     */
    int uninit();

    /**
     * @brief 释放本对象资源
     */
    int release();

private:
    /**
     * @brief 连接信息结构体
     */
    typedef struct {
        int id;
        int fd;
        char ip[16];
        int port;
        int connect_time;
        union net_io_storage storage;
        list_head recv_buff_head;
        int recv_buff_offset;
        int recv_buff_data_len;
        list_head send_buff_head;
        int send_buff_offset;
        int send_buff_data_len;
    } conn_info_t;
    
    typedef struct {
        list_head list;
        char p_addr[0];
    } mem_block_t;

    /**
     * @brief 从空闲链表里取一个内存块到head链表的尾部
     */
    int alloc_mem_block(struct list_head *head);

    /**
     * @brief 把list内存块放到空闲链表的尾部
     */
    int free_mem_block(struct list_head *list);

    /**
     * @brief 判断head链表是否只含有一个内存块
     */
    bool single_mem_block(struct list_head *head);

    /**
     * @brief 合并内存块链表到一个连续的地址空间
     */
    char * merge_mem_block(struct list_head *head);

    /**
     * @brief 获取链表第一个内存块的地址
     */
    char * get_first_mem_block_addr(struct list_head *head);

    /**
     * @brief 获取链表最后一个内存块的地址
     */
    char * get_last_mem_block_addr(struct list_head *head);

    /**
     * @brief 连接信息结构体MAP
     */
    typedef std::map<int, conn_info_t> id_conn_map_t;

    /**
     * @brief id_conn_map_t类型的迭代器
     */
    typedef id_conn_map_t::iterator id_conn_map_iter_t;

    /**
     * @brief 生成新的连接ID
     */
    int genereate_id();

    /**
     * @brief 设置错误信息
     * @param error_no  错误码
     * @param msg  错误信息格式字符串
     */
    void set_error_info(int error_no, const char *msg, ...);
  
    /**
     * @brief 从对端接收数据
     * @return 成功返回接收数据的长度，失败时返回-1，0表示对端关闭连接
     */
    int recv_data(conn_info_t *p_conn_info);

    /**
     * @brief 向对端发送数据
     * @return 成功返回发送数据的长度，失败时返回-1
     */
    int send_data(conn_info_t *p_conn_info);
    
    /**
     * @brief 向对端发送数据
     * @return 成功返回发送数据的长度，失败时返回-1
     */
    int send_data(conn_info_t *p_conn_info, const char *p_data, int data_len);


    bool m_inited;                                         // 是否初始化信息
    int m_id;                                             // 下一次连接可分配的ID
    int m_id_base;                                        // 连接ID生成基质
    int m_id_step;                                        // 连接ID增长步长
    int m_errno;                                          // 上一次出错的错误码 
    char m_errstr[MAX_ERRSTR_LEN];                        // 上一次出错的错误信息
    int m_listen_fd;                                       // 监听地址对应的文件描述符
    int m_epoll_fd;                                        // EPOLL对应的文件描述符 
    id_conn_map_t m_id_conn_map;                           // 连接ID和连接信息的MAP
    i_net_io_event_handler* m_p_net_io_event_handler;      // 网络IO事件处理对象
    i_net_io_notifier *m_p_net_io_notifier;               // 通知机制对象
    int m_flag;                                            // 全局状态标志(现在包括文件描述符使用完的状态标志)
    epoll_event m_events_[MAX_ACTIVE_CLIENT_NUM + 2];      // 事件集合
    conn_info_t m_conn_info;                               // 连接信息结构体

    list_head m_free_mem_head;
    char *m_p_cont_mem;
    int m_cont_mem_len;
};

#endif /* NET_IO_SERVER_H_2011_07_19 */
