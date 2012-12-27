/*
 * =====================================================================================
 *
 *       Filename:  work_proc.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年06月29日 18时23分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_WORK_PROC_H_20110629
#define H_WORK_PROC_H_20110629
#include <stdint.h>
#include <time.h>
#include <limits.h>
#include <map>
#include <set>
extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
}

#include "../../../common/data_structure.h"
#include "../../../common/message.h"
#include "../../../common/pack/c_pack.h"
#include "../mkdir_p.h"
#include "../i_work_proc.h"
#include "../i_ucount.h"


#define LAST_TWO_ID(user_id) ((user_id) % 100)
#define BEFORE_LAST_TWO_ID(user_id)   (((user_id) / 100) % 100)

extern c_pack g_pack;

typedef struct
{
    time_t timestamp;
    uint32_t user_id; 
} file_cmp_t;

class c_file_cmp
{
public:
    bool operator()(const file_cmp_t &key_a, const file_cmp_t &key_b) const
    {   
        if(key_a.timestamp < key_b.timestamp)
        {   
            return true;
        }   

        if(key_a.timestamp == key_b.timestamp)
        {   
            if(key_a.user_id < key_b.user_id)
            {   
                return true;
            }   
        }   

        return false;
    }   
};


class c_work_proc : public i_work_proc
{
public:
    c_work_proc();
    ~c_work_proc();
    int init(int argc, char** argv);
    int uninit();
    int release();
    int process(char *req, int req_len, char **ack, int *ack_len);

private:
    int process_msg_visit_and_thumb(char *p_body, uint8_t *p_is_visit);
    int get_latest_visit(); 
    int get_unread_count(); 

    int load_all_visit_file();
    void save_latest_visit(uint32_t user_id, uint32_t peer_id);
    void unmap_oldest_file(); 
    char *map_new_file(uint32_t user_id);

    inline int get_cur_day()
    {//获得当前的天 ，110810
        uint32_t format_time = 0;
        struct tm tm_time;
        time_t tmp_timestamp = time(NULL);
        if(NULL == localtime_r(&tmp_timestamp, &tm_time))
        {
            return 0;
        }

        format_time = (tm_time.tm_year + 1900 - 2000) * 10000 + (tm_time.tm_mon + 1) * 100 + tm_time.tm_mday;
        return format_time;
    }

    typedef struct
    {
        uint32_t user_id; 
        uint32_t timestamp;
        uint8_t type;
        uint8_t status;
    } __attribute__((__packed__)) visit_info_t;

    typedef struct
    {
        uint8_t count;
        uint8_t index;
        time_t last_time;
        uint8_t unread_count;
        visit_info_t visit_info[0];
    }  __attribute__((__packed__)) latest_visit_t;

    enum
    {
        visit_type = 1,
        thumb_type = 2,
    };

    enum
    {
        MESSAGE_UNREAD = 0,
        MESSAGE_READ = 1,
    };


private:
    uint8_t m_inited;
    uint32_t m_errno;
    uint32_t m_user_id;

    uint32_t m_seq_id;
    uint16_t m_msg_type;

    int m_last_two_id;
    int m_before_last_two_id;
    int m_cur_day;

    char m_ucount_file[PATH_MAX];
    int m_ucount_prefix_len;//路劲前缀的长度

    char m_visit_file[PATH_MAX];
    int m_visit_prefix_len;//路劲前缀的长度

    int m_max_map_file;

    std::map<uint32_t, char *> m_visit_file_map; //保存每个用户的最近访客的文件映射地址
    std::set<file_cmp_t, c_file_cmp> m_file_time_set;  //保存每个文件被用到的时间，做lru算法
    int m_visit_count;  //保存的最近访客的个数

    char m_response_buffer[MAX_SEND_PKG_SIZE];

    i_ucount *m_p_ucount;
};

#endif //H_WORK_PROC_20110629
