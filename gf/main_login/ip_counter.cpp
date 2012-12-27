/**
 *============================================================
 *  @file      ip_counter.cpp
 *  @brief     根据命令号执行IP相关的动作。
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

/*!
 * @mainpage IpCounter文档
 *
 * IpCounter基于AsyncServer框架，其他的程序可通过TCP协议发送4字节的命令号和4字节的IP地址，
 * IPCounter可根据命令号对IP记数并且返回IP出现的次数或清除IP相关的信息或返回IP统计的次数
 * 和第一次记数时的时间。并且每天通过定时器对IP记数清零一次。目前注册服务器利用该功能来实
 * 现限制单个IP每天的最大注册数。
 */

#include <map>

using namespace std;

extern "C" {
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
}
#include "asyc_main_login_type.h"



struct DailyTimer {
    list_head_t timer_list;
} daily_tmr;

/*! 记录每个ip出现的次数，每天零点清空一次 */
map<uint32_t, ip_info_t> ipcnt;

/**
 * @brief 定义命令号
 */
enum cmd_id_t {
    /*! IP记数加一 */
    proto_add_ip = 1,
    /*! 清除IP的信息 */
    proto_clear_ip = 2,
    /*! 返回IP的信息 */
    proto_get_ip = 3
};

/**
 * @brief 清空ipcnt。 
 *
 * @return 总是0。
 */
int purge_ip_cnt(void* owner, void* data)
{
    DEBUG_LOG("TOTAL DIFF IP NUM TODAY\t[%u]", ipcnt.size());

    ipcnt.clear();
    ADD_TIMER_EVENT(&daily_tmr, purge_ip_cnt, 0, get_now_tv()->tv_sec + 86400);
    return 0;
}

int ip_counter_init()
{
    INIT_LIST_HEAD(&daily_tmr.timer_list);

    tm tm_tmp      = *get_now_tm();
    tm_tmp.tm_hour = 0;
    tm_tmp.tm_min  = 0;
    tm_tmp.tm_sec  = 0;
    time_t exptm   = mktime(&tm_tmp) + 86400;
    ADD_TIMER_EVENT(&daily_tmr, purge_ip_cnt, 0, exptm);

    return 0;
}

void ip_counter_clear(uint32_t ip)
{
    ipcnt.erase(ip);
}

void ip_counter_add(uint32_t ip)
{
    ipcnt[ip].ip_cnt++;
}

int ip_counter_info(uint32_t ip, ip_info_t& myinfo)
{
    map<uint32_t, ip_info_t>::iterator iter;
    iter = ipcnt.find(ip);
    if (iter != ipcnt.end()) {
        myinfo.ip_cnt = iter->second.ip_cnt;
        myinfo.first_time = iter->second.first_time;
    } else {
        myinfo.ip_cnt = 0;
        myinfo.first_time = 0;
    }
    return 0;
}

