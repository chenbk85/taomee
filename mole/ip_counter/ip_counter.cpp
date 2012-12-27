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

/**
 * @brief 存储IP统计的次数和第一次统计的时间值
 */
struct ip_info_t {
	/**
	 * @brief 构造函数，初始化ip_cnt为零和first_time为当前时间
	 */
	ip_info_t()
	{ 
		ip_cnt = 0; 
		first_time = time(0); 
	}
	/*! 存储IP地址的次数 */
	uint32_t ip_cnt;
	/*! 存储IP第一次统计的时间 */
	time_t first_time;
} __attribute__((packed));


struct DailyTimer {
	list_head_t timer_list;
} daily_tmr;

/*! 记录每个ip出现的次数，每天零点清空一次 */
map<in_addr_t, ip_info_t> ipcnt;

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

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  init_service(int isparent)
{
	if (!isparent) {
		setup_timer();
		INIT_LIST_HEAD(&daily_tmr.timer_list);

		tm tm_tmp      = *get_now_tm();
		tm_tmp.tm_hour = 0;
		tm_tmp.tm_min  = 0;
		tm_tmp.tm_sec  = 0;
		time_t exptm   = mktime(&tm_tmp) + 86400;
		ADD_TIMER_EVENT(&daily_tmr, purge_ip_cnt, 0, exptm);
	}

	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  fini_service(int isparent)
{
	if (!isparent) {
		destroy_timer();
	}
	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void proc_events()
{
	handle_timer();
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{
	
	if (isparent && avail_len >= 8) {
		return 8;
	}

	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	uint32_t *client_data = reinterpret_cast<uint32_t*>(data);
	uint32_t  cnt = 0;
	map<in_addr_t, ip_info_t>::iterator iter;
	uint32_t cmd_id = ntohl(client_data[0]);
	ip_info_t temp;
	switch (cmd_id) {
	case proto_add_ip: /* 对IP统计次数加一 */
		cnt = htonl(++(ipcnt[client_data[1]].ip_cnt));
		send_pkg_to_client(fdsess, &cnt, 4);
		break;

	case proto_clear_ip: /* 清除相应的IP统计次数 */
		ipcnt.erase(client_data[1]);
		break;
	
	case proto_get_ip: /*发送IP的统计次数和时间*/
		iter = ipcnt.find(client_data[1]);
		if (iter != ipcnt.end()) {
			temp.ip_cnt = htonl(iter->second.ip_cnt);
			temp.first_time = htonl(iter->second.first_time);
		} else {
			temp.ip_cnt = 0;
			temp.first_time = 0;
		}
		send_pkg_to_client(fdsess, &temp, sizeof(ip_info_t));
		break;

	default:
		ERROR_LOG("invalid command id %u", cmd_id);
		return -1;
		break;
	}

	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void proc_pkg_from_serv(int fd, void* data, int len)
{
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void on_client_conn_closed(int fd)
{
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void on_fd_closed(int fd)
{
}

