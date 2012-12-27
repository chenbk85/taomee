/*
 * =====================================================================================
 *
 *       Filename:  online.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月11日 20时40分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <arpa/inet.h>
#include <map>
#include <string>
#include <functional>
#include <string.h>
#include <errno.h>



#include "online.h"

using namespace std;

c_online g_online;

c_online::c_online()
{
}

c_online::~c_online()
{
    uninit();
}

int c_online::init()
{
    m_online_user_num = 0;
    m_tel_online_num = 0;
    m_net_online_num = 0;
    m_online_last_minute = 0;
    m_user_info_map.clear();
    m_online_info_map.clear();
    m_fd_online_id_map.clear();

    memset(&m_stat_login_logout, 0, sizeof(m_stat_login_logout));

    memset(m_send_buffer, 0, sizeof(m_send_buffer));
    m_send_buffer_len = 0;
    m_p_send_header = (svr_msg_header_t*)m_send_buffer;

    m_errno = ERR_NO_ERR;
    m_userid = 0;
    m_msg_type= 0;

    if((m_stat_file = config_get_strval("stat_file")) == NULL)
    {
        ERROR_LOG("read stat_file config failed.");
        return -1;
    }

    m_warning_line = config_get_intval("warning_line", 0);
    if(m_warning_line <= 0)
    {
        ERROR_LOG("read warning line config failed.");
        return -1;
    }


    struct sockaddr_in bind_addr;
    if(inet_pton(AF_INET, config_cache.bc_elem->bind_ip, &bind_addr.sin_addr) <= 0)
    {
        ERROR_LOG("inet_pton %s failed(err:%s).", config_cache.bc_elem->bind_ip, strerror(errno));
        return -1;
    }

    m_bind_ip = bind_addr.sin_addr.s_addr;


    DEBUG_LOG("start init stat_online timer ...");
    //初始化统计在线人数的定时器
    INIT_LIST_HEAD(&(m_stat_online_tmr.timer_list));
    ADD_TIMER_EVENT(&m_stat_online_tmr, c_online::wrapper_stat_user_num, this, get_now_tv()->tv_sec + 60);
    DEBUG_LOG("finished init stat_online timer.");

    m_inited = 1;
    return 0;
}

int c_online::uninit()
{
    if(!m_inited)
    {
        return -1;
    }

    m_online_user_num = 0;
    m_tel_online_num = 0;
    m_net_online_num = 0;
    memset(&m_stat_login_logout, 0, sizeof(m_stat_login_logout));
    m_user_info_map.clear();
    m_online_info_map.clear();
    m_fd_online_id_map.clear();

    memset(m_send_buffer, 0, sizeof(m_send_buffer));
    m_p_send_header = NULL;

    m_inited = 0;
    return 0;
}

/**
 * @brief register_online online向switch发注册请求
 *          正常情况下，无需返回包
 * @param p_recv_header
 * @param body_len
 * @param fdsess
 *
 * @return
 */
int c_online::register_online(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
	m_errno = ERR_NO_ERR;
	m_userid = p_recv_header->user_id;
	m_msg_type = p_recv_header->msg_type;
	m_send_buffer_len = sizeof(svr_msg_header_t);

	//uint32_t *online_id = (uint32_t *)p_recv_header->body;
	online_register_switch_t *p_online_reg = (online_register_switch_t *)p_recv_header->body;
	map<uint32_t, online_info_t>::iterator iter = m_online_info_map.find(p_online_reg->online_id);

	m_errno = check_val_len(sizeof(online_register_switch_t), body_len);
	if(m_errno != 0)
	{
		goto end;
	}


	if(iter != m_online_info_map.end())
	{//正常情况时不应该走到这里面，如果不幸走到这里，说明有online重复发起注册请求
		WARN_LOG("[regiter_online]recv ip:%s port:%u fd:%d id:%u net:%u(0:tel 1:net) repeat register.", p_online_reg->online_ip, p_online_reg->online_port, fdsess->fd, p_online_reg->online_id, p_online_reg->zone);
		//m_errno = ERR_REPEAT_REQUEST;
	}
	else
	{
		INFO_LOG("[register_online]recv ip:%s port:%u fd:%d id:%u net:%u(0:tel 1:net) normal register.", p_online_reg->online_ip, p_online_reg->online_port, fdsess->fd, p_online_reg->online_id, p_online_reg->zone);
		online_info_t online;
		online.online_id = p_online_reg->online_id;
		online.conn_num = 0;
        online.cur_index = 0;
        memset(online.user_ids, 0, MAX_NUM_PER_ONLINE * sizeof(uint32_t));
		memcpy(online.online_ip, p_online_reg->online_ip, 16);
		online.online_port = p_online_reg->online_port;
		online.fdsess = fdsess;
        online.online_zone = p_online_reg->zone;

		m_online_info_map.insert(pair<uint32_t, online_info_t>(p_online_reg->online_id, online));
		m_fd_online_id_map[fdsess->fd] = p_online_reg->online_id;

        INIT_LIST_HEAD(&(m_online_keepalive_tmr[online.online_id - 1].timer_list));//online_id从1开始编号
        m_online_keepalive_tmr[online.online_id - 1].tmr = ADD_TIMER_EVENT(&m_online_keepalive_tmr[online.online_id - 1], c_online::online_keepalive_check, (void*)(&online), get_now_tv()->tv_sec + 70);

	}

end:
	return 0;
}


int c_online::online_keepalive_check(void *owner, void *p_data)
{
    online_info_t *p_online = (online_info_t*)p_data;
    if(p_online == NULL)
    {
        return 0;
    }
	if(p_online->fdsess == NULL)
	{
		return 0;
	}

    WARN_LOG("keepalive check expired. online closed[id:%d, fd:%d ip:%s, port:%u net:%u(0:tel 1:net)]", p_online->online_id, p_online->fdsess->fd, p_online->online_ip, p_online->online_port, p_online->online_zone);
    close_client_conn(p_online->fdsess->fd);
   // online_closed(p_online->fdsess->fd);
    return 0;
}

int c_online::online_closed(int fd)
{
    uint32_t online_id = m_fd_online_id_map[fd];
    m_fd_online_id_map.erase(fd);
	if(online_id < 1)
	{
			return 0;
	}
    map<uint32_t, online_info_t>::iterator iter = m_online_info_map.find(online_id);
    if(iter != m_online_info_map.end())
    {
        INFO_LOG("[online_closed]ip:%s port:%u fd:%d id:%u net:%u(0:tel 1:net) closed connnection.", iter->second.online_ip, iter->second.online_port, fd, iter->second.online_id, iter->second.online_zone);

    m_online_keepalive_tmr[online_id - 1].tmr = 0;
        REMOVE_TIMERS(&m_online_keepalive_tmr[online_id - 1]);

        m_online_info_map.erase(iter);

    }
    else
    {
        WARN_LOG("[online_closed]Not found fd:%d online_id:%u in online_map.", fd, online_id);
    }
    return 0;
}

int c_online::make_online_keepalive(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    int online_id = m_fd_online_id_map[fdsess->fd];
    if(online_id > 0)
    {
        mod_expire_time(m_online_keepalive_tmr[online_id - 1].tmr, get_now_tv()->tv_sec + 70 );
    }
    return 0;
}
/**
 * @brief clear_register_online 清除注册信息
 *
 * @param fd
 *
 * @return
 */
int c_online::clear_register_online(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
	m_errno = ERR_NO_ERR;
	m_userid = p_recv_header->user_id;
	m_msg_type = p_recv_header->msg_type;
	m_send_buffer_len = sizeof(svr_msg_header_t);

	uint32_t online_id = m_fd_online_id_map[fdsess->fd];
	m_fd_online_id_map.erase(fdsess->fd);

	if(online_id == 0)
	{
			return 0;
	}

	map<uint32_t, online_info_t>::iterator iter = m_online_info_map.find(online_id);
	if(iter != m_online_info_map.end())
	{
		INFO_LOG("[clear_register_online]ip:%s port:%u fd:%d id:%u net:%u(0:tel 1:net) exit.", iter->second.online_ip, iter->second.online_port, fdsess->fd, iter->second.online_id, iter->second.online_zone);
		for(uint16_t index = 0; index < MAX_NUM_PER_ONLINE; index++)
		{
			uint32_t user_id = iter->second.user_ids[index];
			if(user_id != 0)
			{
				m_user_info_map.erase(user_id);
			}
		}


        if(iter->second.online_zone == TEL_COM)
        {
            m_tel_online_num -= iter->second.conn_num;
		    m_online_user_num -= iter->second.conn_num; //在线人数减少
            if((int)(m_tel_online_num) < 0)
            {
                m_tel_online_num = 0;
            }
		    if((int)m_online_user_num < 0)
		    {//防止出现负数
			    m_online_user_num = 0;
		    }
        }
        else if(iter->second.online_zone == NET_COM)
        {
            m_net_online_num -= iter->second.conn_num;
		    m_online_user_num -= iter->second.conn_num; //在线人数减少
            if((int)(m_net_online_num) < 0)
            {
                m_net_online_num = 0;
            }
		    if((int)m_online_user_num < 0)
		    {//防止出现负数
			    m_online_user_num = 0;
		    }
        }

		m_online_info_map.erase(iter);

        m_online_keepalive_tmr[online_id - 1].tmr = 0;
		REMOVE_TIMERS(&m_online_keepalive_tmr[online_id - 1]);
	}
	else
	{
		WARN_LOG("[clear_register_online]Not found fd:%d online_id:%u in online_map.", fdsess->fd, online_id);
	}
	return 0;
}

/**
 * @brief acquire_available_online 获取一个可用的online服务器
 *
 * @param p_recv_header
 * @param body_len
 * @param fdsess
 *
 * @return
 */
int c_online::acquire_available_online(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    m_errno = ERR_NO_ERR;
    m_userid = p_recv_header->user_id;
    m_msg_type= p_recv_header->msg_type;

    m_errno = check_val_len(sizeof(uint8_t), body_len);
    if(m_errno != 0)
    {
        goto end;
    }

    {
        m_send_buffer_len = sizeof(svr_msg_header_t);

        uint8_t tel_or_net = *(uint8_t*)p_recv_header->body;
        INFO_LOG("recv user %u net %u(0:tel 1:net) acquire availbel online", m_userid, tel_or_net);

        uint32_t least_num = 0;
        map<uint32_t, online_info_t>::iterator least_iter;
        map<uint32_t, online_info_t>::iterator iter = m_online_info_map.begin();
        if(iter != m_online_info_map.end())
        {
            least_num = iter->second.conn_num;
            if(iter->second.online_zone != tel_or_net)
            {
                iter++;
                least_num = iter->second.conn_num;
            }

            least_iter = iter;
            iter++;
            for(; iter != m_online_info_map.end(); iter++)
            {
                if(iter->second.online_zone != tel_or_net)
                {
                    continue;
                }
                if(iter->second.conn_num < least_num)
                {
                    least_num = iter->second.conn_num;
                    least_iter = iter;
                }
            }

            INFO_LOG("least num:%u", least_num);
            if(least_num > MAX_NUM_PER_ONLINE*3/4)
            {//当单台online的人数达到总人数的3/4时报警，这时需要加机器
                WARN_LOG("online num reached 3/4, need add machine.");
                asynsvr_send_warning_msg("monster need add machine", 0, 0, 0, "192.168.61.238");
            }

            if(least_num >= MAX_NUM_PER_ONLINE)
            {
                ERROR_LOG("[acquire_available_online]: all online is full.");
                asynsvr_send_warning_msg("all monster online is full, add machine quickly!!!", 0, 0, 0, "192.168.61.238");
                m_errno = ERR_NO_AVAILABLE_ONLINE;
                goto end;
            }

            if((m_errno = pack((void*)(&(least_iter->second.online_ip)), 16)) != 0)
            {
                ERROR_LOG("[acquire_available_online]: send buffer overflowed(m_send_buffer_len:%u, pack_len:16).", m_send_buffer_len);
                goto end;
            }

            if((m_errno = pack((void*)(&(least_iter->second.online_port)), 2)) != 0)
            {
                ERROR_LOG("[acquire_available_online]: send buffer overflowed(m_send_buffer_len:%u, pack_len:2).", m_send_buffer_len);
                goto end;
            }
        }
        else
        {//当前没有可用的online服务器
            m_errno = ERR_NO_AVAILABLE_ONLINE;
            ERROR_LOG("[acquire_available_online]no online available");
        }
    }
end:

    return send_to_client(fdsess, p_recv_header->seq_id, p_recv_header->msg_type);
}

/**
 * @brief login_online 用户登录进online 需要判断用户是否在其他online登录中，如果是，需要通知相应online将用户踢下线
 *          正常情况下，无需返回包
 * @param p_recv_header
 * @param body_len
 * @param fdsess
 *
 * @return
 */
int c_online::login_online(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    m_errno = ERR_NO_ERR;
    m_userid = p_recv_header->user_id;
    m_msg_type= p_recv_header->msg_type;
    INFO_LOG("recv user:%u login.", m_userid);

    m_send_buffer_len = sizeof(svr_msg_header_t);

    uint32_t new_online_id = get_online_id(fdsess->fd);
    if(new_online_id == (uint32_t)-1)
    {
        ERROR_LOG("user %u get_online_id=-1 fd=%d", m_userid, fdsess->fd);
        goto end;
    }

    {
        map<uint32_t, user_info_t>::iterator iter = m_user_info_map.find(m_userid);
        if(iter != m_user_info_map.end())
        {//用户已经登录
            uint32_t old_online_id = iter->second.online_id;
            uint16_t old_index = iter->second.online_index;
            if(new_online_id == old_online_id)
            {//It should not come here
                WARN_LOG("user:%u login at the same online:%u", m_userid, new_online_id);
        //        m_errno = ERR_REPEAT_LOGIN;
            }
            else
            {//告知online将用户踢下线
                m_errno = ERR_LOGIN_AT_OTHER;
                send_to_client(m_online_info_map[old_online_id].fdsess, 0, svr_msg_kick_user);

                if(m_online_info_map[old_online_id].online_zone != m_online_info_map[new_online_id].online_zone)
                {//跨区重复登录
                    if(m_online_info_map[old_online_id].online_zone == TEL_COM)
                    {
                        m_tel_online_num--;
                        if(m_tel_online_num < 0)
                        {
                            m_tel_online_num = 0;
                        }
                        m_net_online_num++;
                        WARN_LOG("user %u have login at other online:%u old_zone:TEL_COM", m_userid, old_online_id);
                    }
                    else
                    {
                        m_tel_online_num++;
                        m_net_online_num--;
                        if(m_net_online_num < 0)
                        {
                            m_net_online_num = 0;
                        }
                        WARN_LOG("user %u have login at other online:%u old_zone:NET_COM", m_userid, old_online_id);

                    }
                }
                else
                {
                    WARN_LOG("user %u have login at other online:%u", m_userid, old_online_id);
                }

                m_online_info_map[old_online_id].conn_num -= 1;//旧的online上的人数减一
                m_online_info_map[old_online_id].user_ids[old_index] = 0;
                m_errno = ERR_NO_ERR;

                uint16_t new_index = add_user_to_online_map(new_online_id, m_userid);
                if(new_index == -1)
                {
                    goto end;
                }

                iter->second.login_time = time(NULL);
                iter->second.online_id = new_online_id;
                iter->second.online_index = new_index;




            }
        }
        else
        {
            uint16_t new_index = add_user_to_online_map(new_online_id, m_userid);
            if(new_index == -1)
            {
                goto end;
            }
            user_info_t user_info;
            user_info.user_id = m_userid;
            user_info.login_time = time(NULL);
            user_info.online_id = new_online_id;
            user_info.online_index = new_index;
            m_user_info_map.insert(pair<uint32_t, user_info_t>(m_userid, user_info));


            if(m_online_info_map[new_online_id].online_zone == TEL_COM)
            {
                m_tel_online_num++;
            }
            else
            {
                m_net_online_num++;
            }
            m_online_user_num++;//在线人数++

            //统计登录登出
            m_stat_login_logout.value_1 = 1;
            m_stat_login_logout.value_2 = 0;
            m_stat_login_logout.value_3 = m_userid;
            m_stat_login_logout.value_4 = 0;

            uint32_t now = get_now_tv()->tv_sec;
            msglog(m_stat_file, stat_login_logout, now, &m_stat_login_logout, sizeof(m_stat_login_logout));
        }
    }
end:
    return 0;
}

/**
 * @brief logout_online 用户登出online
 *          正常情况下，无需返回包
 *
 * @param p_recv_header
 * @param body_len
 * @param fdsess
 *
 * @return
 */
int c_online::logout_online(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    m_errno = ERR_NO_ERR;
    m_userid = p_recv_header->user_id;
    m_msg_type= p_recv_header->msg_type;

    m_send_buffer_len = sizeof(svr_msg_header_t);

    map<uint32_t, user_info_t>::iterator iter = m_user_info_map.find(m_userid);
    if(iter != m_user_info_map.end())
    {
        INFO_LOG("recv user:%u logout.", m_userid);
        uint32_t online_id = iter->second.online_id;

        uint32_t new_online_id = get_online_id(fdsess->fd);
        if(new_online_id == (uint32_t)-1)
        {
            ERROR_LOG("user logout %u get_online_id=-1 fd=%d", m_userid, fdsess->fd);
        }

        if(online_id != new_online_id)
        {
            WARN_LOG("logout msg is delayed, user:%u", m_userid);
            return 0;
        }

        uint16_t online_index = iter->second.online_index;

        m_online_info_map[online_id].conn_num -= 1; //相应online上的人数减一
        m_online_info_map[online_id].user_ids[online_index] = 0;

        if(m_online_info_map[online_id].online_zone == TEL_COM)
        {
              m_tel_online_num--;
              if(m_tel_online_num < 0)
              {
                  m_tel_online_num = 0;
              }
        }
        else
        {
             m_net_online_num--;
             if(m_net_online_num < 0)
             {
                 m_net_online_num = 0;
             }
        }
        m_online_user_num--; //在线人数--
        if(m_online_user_num < 0)
        {
            m_online_user_num = 0;
        }

        uint32_t now = get_now_tv()->tv_sec;
        //统计登录登出
        m_stat_login_logout.value_1 = 0;
        m_stat_login_logout.value_2 = 1;
        m_stat_login_logout.value_3 = m_userid;
        m_stat_login_logout.value_4 = now - iter->second.login_time;
        msglog(m_stat_file, stat_login_logout, now, &m_stat_login_logout, sizeof(m_stat_login_logout));

        m_user_info_map.erase(iter);
    }
    else
    {//用户已经不在线
        m_errno = ERR_USER_UNONLINE;
        ERROR_LOG("user:%u is not online.", m_userid);
    }

    return 0;
}


int c_online::add_friend_request(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    m_errno = ERR_NO_ERR;
    m_userid = p_recv_header->user_id;
    m_msg_type= p_recv_header->msg_type;

    m_send_buffer_len = sizeof(svr_msg_header_t) + sizeof(online_notify_t);

    m_errno = check_val_len(sizeof(online_notify_t), body_len);
    if(m_errno != 0)
    {
        //do nothing
    }
    else
    {
        online_notify_t *p_notify = (online_notify_t *)p_recv_header->body;
        INFO_LOG("recv user %u request user:%u,type:%u.", m_userid, p_notify->user_id, p_notify->type);

        map<uint32_t, user_info_t>::iterator iter = m_user_info_map.find(p_notify->user_id);
        if(iter != m_user_info_map.end())
        {//好友在线
            uint32_t friend_online_id = iter->second.online_id;
            m_userid = p_notify->user_id;
            m_errno = ERR_NO_ERR;
            p_notify->user_id = p_recv_header->user_id;
            map<uint32_t, online_info_t>::iterator online_iter = m_online_info_map.find(friend_online_id);
            if (online_iter != m_online_info_map.end())
            {
	       	    memcpy(m_send_buffer + sizeof(svr_msg_header_t), (char *)p_notify, sizeof(online_notify_t));
                send_to_client(m_online_info_map[friend_online_id].fdsess, 0, svr_msg_online_friend_apply);
                INFO_LOG("notify user:%u, type:%u, peer_id:%u", m_userid, p_notify->type, p_notify->user_id);
            }
            else
            {
                CRIT_LOG("cann't find online id:%u in map.", friend_online_id);
            }
        }
        else
        {//好友不在线, 不需要通知
            INFO_LOG("friend %u is not online", p_notify->user_id);
        }
    }

    return 0;
}

int c_online::get_active_user(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    m_errno = ERR_NO_ERR;
    m_userid = p_recv_header->user_id;
    m_msg_type= p_recv_header->msg_type;
    m_send_buffer_len = sizeof(svr_msg_header_t);

    KINFO_LOG(m_userid, "get active user");
    switch_msg_active_user_t *p_active_user = (switch_msg_active_user_t*)(m_send_buffer + m_send_buffer_len);
    int index = 0;
    for(map<uint32_t, user_info_t>::iterator iter = m_user_info_map.begin(); iter != m_user_info_map.end(); iter++)
    {
        //KINFO_LOG(m_userid, "get user:%u", iter->first);
        p_active_user->user[index].user_id = iter->first;
        p_active_user->user[index].is_vip = 0;
        index++;
        if(index >= MAX_ACTIVE_USER_NUM)
        {
            break;
        }
    }
    p_active_user->count = index;

    m_send_buffer_len += sizeof(switch_msg_active_user_t) + index * sizeof(user_vip_t);

    return send_to_client(fdsess, p_recv_header->seq_id, p_recv_header->msg_type);
}


int c_online::query_switch_status(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    m_errno = ERR_NO_ERR;
    m_userid = p_recv_header->user_id;
    m_msg_type= p_recv_header->msg_type;

    m_send_buffer_len = sizeof(svr_msg_header_t);


    if(fdsess->remote_ip != m_bind_ip)
    {
        WARN_LOG("Just allow bind_ip(%s:%u) to connect(remote_ip:%u).", config_cache.bc_elem->bind_ip, m_bind_ip, fdsess->remote_ip);
        m_errno = ERR_NOT_ALLOWED;
    }
    else
    {
        switch_online_info_t *p_sw_online = (switch_online_info_t*)(m_send_buffer + m_send_buffer_len);
        p_sw_online->cur_user_num = m_online_user_num;

        map<string, uint32_t> ip_num_map;
        for(map<uint32_t, online_info_t>::iterator iter = m_online_info_map.begin(); iter != m_online_info_map.end(); iter++)
        {
            map<string, uint32_t>::iterator ip_iter = ip_num_map.find(iter->second.online_ip);
            if(ip_iter == ip_num_map.end())
            {
                ip_num_map.insert(pair<string, uint32_t>(iter->second.online_ip, iter->second.conn_num));
            }
            else
            {
                ip_iter->second += iter->second.conn_num;
            }
        }

        p_sw_online->ip_num = ip_num_map.size();
        int i = 0;
        for(map<string, uint32_t>::iterator tmp_iter = ip_num_map.begin(); tmp_iter != ip_num_map.end(); tmp_iter++)
        {
            strcpy(p_sw_online->ip_info[i].online_ip, (tmp_iter->first).c_str());
            p_sw_online->ip_info[i].online_num = tmp_iter->second;
            i++;
        }

        m_send_buffer_len += (sizeof(switch_online_info_t) + p_sw_online->ip_num * sizeof(ip_info_t));
    }

    return send_to_client(fdsess, 0, svr_msg_query_switch_status);

}

int c_online::query_ip_info(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    m_errno = ERR_NO_ERR;
    m_userid = p_recv_header->user_id;
    m_msg_type= p_recv_header->msg_type;

    m_send_buffer_len = sizeof(svr_msg_header_t);

    m_errno = check_val_len(16, body_len);
    if(m_errno != 0)
    {
        //do nothing
    }
    else
    {
        if(fdsess->remote_ip != m_bind_ip)
        {
            WARN_LOG("Just allow bind_ip(%s:%u) to connect(remote_ip:%u).", config_cache.bc_elem->bind_ip, m_bind_ip, fdsess->remote_ip);
            m_errno = ERR_NOT_ALLOWED;
        }
        else
        {
            char online_ip[16] = {0};
            strcpy(online_ip, p_recv_header->body);

            online_ip_t *p_sw_ip = (online_ip_t*)(m_send_buffer + m_send_buffer_len);
            int i = 0;
            for(map<uint32_t, online_info_t>::iterator iter = m_online_info_map.begin(); iter != m_online_info_map.end(); iter++)
            {
                if(strcmp(online_ip, iter->second.online_ip) == 0)
                {
                    p_sw_ip->online[i].online_id = iter->second.online_id;
                    p_sw_ip->online[i].online_fd = iter->second.fdsess->fd;
                    p_sw_ip->online[i].conn_num = iter->second.conn_num;
                    p_sw_ip->online[i].cur_index = iter->second.cur_index;
                    p_sw_ip->online[i].online_port = iter->second.online_port;
                    i++;
                }
                else
                {
                    continue;
                }
            }

            p_sw_ip->online_num = i;
            m_send_buffer_len += (sizeof(online_ip_t) + p_sw_ip->online_num * sizeof(online_t));
        }
    }

    return send_to_client(fdsess, 0, svr_msg_query_ip);
}

int c_online::query_user_info(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    m_errno = ERR_NO_ERR;
    m_userid = p_recv_header->user_id;
    m_msg_type= p_recv_header->msg_type;

    m_send_buffer_len = sizeof(svr_msg_header_t);

    m_errno = check_val_len(4, body_len);
    if(m_errno != 0)
    {
        //do nothing
    }
    else
    {
        if(fdsess->remote_ip != m_bind_ip)
        {
            WARN_LOG("Just allow bind_ip(%s:%u) to connect(remote_ip:%u).", config_cache.bc_elem->bind_ip, m_bind_ip, fdsess->remote_ip);
            m_errno = ERR_NOT_ALLOWED;
        }
        else
        {
            uint32_t user_id = *(uint32_t*)(p_recv_header->body);
            switch_user_t *p_usr = (switch_user_t*)(m_send_buffer + m_send_buffer_len);
            map<uint32_t, user_info_t>::iterator iter = m_user_info_map.find(user_id);
            if(iter != m_user_info_map.end())
            {
                p_usr->is_online = 1;
                p_usr->login_time = iter->second.login_time;
                p_usr->online_id = iter->second.online_id;
                p_usr->online_port = m_online_info_map[p_usr->online_id].online_port;
                strcpy(p_usr->online_ip, m_online_info_map[p_usr->online_id].online_ip);
            }
            else
            {
                p_usr->is_online = 0;
            }

            m_send_buffer_len += sizeof(switch_user_t);

        }
    }

    return send_to_client(fdsess, 0, svr_msg_query_user);
}


int c_online::wrapper_stat_user_num(void *owner, void *p_data)
{
    c_online *p_obj = (c_online*)p_data;
    return p_obj->stat_user_num();
}


int c_online::stat_user_num()
{
    uint32_t now = get_now_tv()->tv_sec;

    msglog(m_stat_file, stat_online_usernum, now, &m_tel_online_num, 4);
    msglog(m_stat_file, stat_online_total, now, &m_online_user_num, 4);
    msglog(m_stat_file, stat_online_usernum_netcom, now, &m_net_online_num, 4);

   if((int)(m_online_last_minute - m_online_user_num) > m_warning_line)
    {//在线人数下滑 短信报警
        for(int i = 0; i < 12; i++)
        {//一分钟内调用该接口上报同一个svr出错超过10次（比如上报switch出错超过10次），才会收到报警短信
            asynsvr_send_warning_msg("monster online drop down ...", 0, 0, 0, "192.168.61.238");
        }
    }
    m_online_last_minute = m_online_user_num;
    ADD_TIMER_EVENT(&m_stat_online_tmr, c_online::wrapper_stat_user_num, this, get_now_tv()->tv_sec + 60);
    return 0;
}

int c_online::pack(const void *val, uint32_t val_len)
{
    if(m_send_buffer_len + val_len > MAX_SEND_PKG_SIZE)
    {
        return ERR_SNDBUF_OVERFLOW;
    }

    memcpy(m_send_buffer + m_send_buffer_len, val, val_len);
    m_send_buffer_len += val_len;
    return 0;
}

int c_online::send_to_client(fdsession_t *fdsess, uint32_t seq_id, uint16_t msg_type)
{
    if(fdsess == NULL)
    {
        KCRIT_LOG(0, "send_to_client fdsess is NULL");
        return 0;
    }

    if(m_errno != ERR_NO_ERR)
    {
        m_send_buffer_len = sizeof(svr_msg_header_t);
    }

    m_p_send_header->len = m_send_buffer_len;
    m_p_send_header->seq_id = seq_id;
    m_p_send_header->msg_type = msg_type;
    m_p_send_header->result = m_errno;
    m_p_send_header->user_id = m_userid;

    if(0 != send_pkg_to_client(fdsess, m_send_buffer, m_send_buffer_len))
    {
        ERROR_LOG("send msg_type:%u to user:%u client failed.", msg_type, m_userid);
    }
    return 0;
}

