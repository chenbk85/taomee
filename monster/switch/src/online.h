/*
 * =====================================================================================
 *
 *       Filename:  online.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月11日 20时21分02秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_ONLINE_H_20110711
#define H_ONLINE_H_20110711
#include <stdint.h>
#include <map>

extern "C"
{
    #include <libtaomee/log.h>
    #include <libtaomee/timer.h>
    #include <libtaomee/list.h>
    #include <libtaomee/project/stat_agent/msglog.h>
    #include <libtaomee/conf_parser/config.h>
    #include <async_serv/net_if.h>
}

#include "switch.h"
#include "../../common/message.h"
#include "../../common/constant.h"
#include "../../common/data_structure.h"
#include "../../common/stat.h"


#pragma pack(push)
#pragma pack(1)
typedef struct
{
    uint32_t user_id;
    uint32_t login_time;        //登录上来的时间
    uint32_t online_id;        //online_id
    uint16_t online_index;      //在online数组中的下标位置
}user_info_t;

typedef struct
{
    uint32_t online_id;         //online id
    uint32_t conn_num;          //连接到该online上的用户数量
    uint16_t cur_index;         //当前使用到的下标，循环使用
    uint32_t user_ids[MAX_NUM_PER_ONLINE]; //记录连接到该online上的用户米米号
    char online_ip[16];         //online ip
    uint16_t online_port;       //online port
    fdsession_t *fdsess;
    uint8_t online_zone; //电信用户or网通用户
}online_info_t;

/**
 * @brief 统计在线人数的定时器
 */
typedef struct
{
    list_head_t timer_list;
}online_stat_timer_t;

typedef struct
{
	timer_struct_t *tmr;
	list_head_t timer_list;
}online_keepalive_timer_t;

#pragma pack(pop)

class c_online
{
public:
    c_online();
    ~c_online();
    int init();
    int uninit();

public:
    //以下均是来自online服务器的协议处理函数
    //50101 online发注册请求信息
    int register_online(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);
    //50102 有用户登录上online,告知switch
    int login_online(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);
    //50103 有用户登出online,告知switch
    int logout_online(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);
    //50104 online通知switch有添加好友的请求
    int add_friend_request(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);

    int query_switch_status(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);
    int query_ip_info(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);
    int query_user_info(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);

    //50105 online从switch处获取活跃用户id
    int get_active_user(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);

    int make_online_keepalive(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);
    int clear_register_online(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);

public:
    //以下均是来自login服务器的协议处理函数
    //50001 login发拉取一个可用online服务请求
    int acquire_available_online(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *p_fdsess);

public:
    //内部调用的一些函数
    int online_closed(int fd);

protected:
    static int wrapper_stat_user_num(void *owner, void* p_data);
    static int online_keepalive_check(void *owner, void *p_data);

    int stat_user_num();
private:
    //封装一些私有函数
    int pack(const void* val, uint32_t val_len);
    int send_to_client(fdsession_t *fdsess, uint32_t seq_id, uint16_t msg_type);

    /**
     * @brief check_val_len 检查长度是否正确
     *
     * @param real_len  实际应该的长度
     * @param recved_len 收到的数据长度
     *
     * @return
     */
    inline int check_val_len(uint32_t real_len, uint32_t recved_len)
    {
        if(real_len != recved_len)
        {
           ERROR_LOG("[msg_type:%u]recved_len(%u) is not equal to real_len(%u).", m_msg_type, recved_len, real_len);
           return ERR_MSG_LEN;
        }
        return 0;
    }

    /**
     * @brief get_online_id 根据fd查找对应的online_id
     *
     * @param fd
     *
     * @return -1没有找到
     */
    inline uint32_t get_online_id(int fd)
    {
        std::map<int, uint32_t>::iterator iter = m_fd_online_id_map.find(fd);
        if(iter == m_fd_online_id_map.end())
        {
            m_errno = ERR_SYSTEM_ERR;
            return -1;
        }
        else
        {
            return iter->second;
        }
    }


    /**
     * @brief add_user_to_online_map 返回在online_map的用户数组中的下标位置
     *
     * @param online_id
     * @param user_id
     *
     * @return
     */
    inline uint16_t add_user_to_online_map(uint32_t online_id, uint32_t user_id)
    {
        if(online_id == (uint32_t)-1 || user_id == 0)
        {
            ERROR_LOG("para error:online_id = %u, user_id = %u.", online_id, user_id);
            return -1;
        }
        int count = 0;
        uint16_t new_index = m_online_info_map[online_id].cur_index;
        if(new_index >= MAX_NUM_PER_ONLINE)
        {
            CRIT_LOG("new_index(%u) >= MAX_NUM_PER_ONLINE(%d).", new_index, MAX_NUM_PER_ONLINE);
        }

        while(new_index < MAX_NUM_PER_ONLINE && m_online_info_map[online_id].user_ids[new_index] != 0)
        {
            new_index++;
            if(new_index == MAX_NUM_PER_ONLINE)
            {
                count++;
                if(count == 2)
                {//不可能两次进入这里  防止死循环
                    break;
                }
                new_index = 0;
            }
        }

        if(count == 2)
        {
            ERROR_LOG("It's not possible to come here.");
            return -1;
        }

	 INFO_LOG("conn num:%u", m_online_info_map[online_id].conn_num);
         m_online_info_map[online_id].conn_num += 1;//相应online上的人数加一
         m_online_info_map[online_id].user_ids[new_index] = m_userid;
         m_online_info_map[online_id].cur_index = new_index;

         return new_index;
    }

private:
    uint8_t m_inited;

    std::map<uint32_t, user_info_t> m_user_info_map;            //记录每个用户的连接信息, user_id作为key
    std::map<uint32_t, online_info_t> m_online_info_map;             //记录每台online服务器的信息, online_id作为key
    std::map<int, uint32_t> m_fd_online_id_map;                 //记录fd对应的online_id


    char m_send_buffer[MAX_SEND_PKG_SIZE];                      //发送缓冲
    uint32_t m_send_buffer_len;
    svr_msg_header_t *m_p_send_header;                               //指向发送缓冲



    uint32_t m_errno;
    uint32_t m_userid;
    uint16_t m_msg_type;

    char* m_stat_file;                                  //统计日志文件
    uint32_t m_online_user_num;                         //记录当前在线用户数量
    uint32_t m_tel_online_num;                          //记录电信在线用户
    uint32_t m_net_online_num;                          //记录网通在线人数

    stat_four_param_t m_stat_login_logout;              //统计登录登出
    uint32_t m_online_last_minute;                      //上一分钟在线人数
    int m_warning_line;                                 //前后两分钟在线人数相差超过该值就报警
    online_stat_timer_t m_stat_online_tmr;              //统计在线人数的定时器
    online_keepalive_timer_t m_online_keepalive_tmr[MAX_ONLINE_NUM];           //检测各个online是否活跃的定时器

    uint32_t m_bind_ip;                                 //switch服务器绑定到的ip
};

#endif //H_ONLINE_H_20110711
