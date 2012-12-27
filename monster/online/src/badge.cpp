/*
 * =====================================================================================
 *
 *       Filename:  badge.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年02月20日 18时09分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */

#include <map>

#include "c_user_cache.h"
#include "online_structure.h"
#include "message.h"
#include "constant.h"
#include "function.h"
#include "cli_proto.h"
#include "data.h"
#include "badge.h"
#include "stat.h"


using namespace std;

int send_badge_to_db(usr_info_t *p_user, badge_info_t badge)
{
    KINFO_LOG(p_user->uid, "cache badge_id:%u, status:%u, progress:%u", badge.badge_id, badge.badge_status, badge.badge_progress);
    pack_as_pkg_header(p_user->uid, badge_update_status, 0, ERR_NO_ERR);
    g_send_msg.pack(badge.badge_id);
    g_send_msg.pack(badge.badge_status);
    g_send_msg.pack(badge.badge_progress);
    g_send_msg.end();
    cache_a_pkg(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
    return 0;
}


int badge_step(usr_info_t *p_user, uint32_t badge_id, uint32_t cond_val)
{
//先判断是否已经获得过这项成就，如果已经获得，无需处理

//    KINFO_LOG(p_user->uid, "badge_step id:%u, value:%u", badge_id, cond_val);
    badge_info_t badge;
    badge.badge_id = badge_id;
    badge.badge_status = badge_running;
    badge.badge_progress = cond_val;

    map<uint32_t, badge_xml_info_t>::iterator iter = g_badge_item_map.find(badge.badge_id);
    if(iter != g_badge_item_map.end())
    {
        uint8_t need_update = iter->second.badge_progress_type;//作为参数传进去时存的是成就项的进度类型
        uint8_t badge_type = iter->second.badge_type;//成就项类型，累加类型or设置类型
        int badge_status = p_user->user_cache.is_badge_acquired(&badge, &need_update, badge_type);
        if(badge_status == -1)
        {//成就项未做缓存
            KCRIT_LOG(p_user->uid, "all badge is not cached, It's not possible!");
            return -1;
        }
        else if(badge_status != badge_running)
        {//成就项已获得, 不需要做任何操作
            return 0;
        }
        else
        {
            //need_update== 1 表示是新加入的成就项
            if(need_update != 1 && badge_type == BADGE_TYPE_SUM)
            {//累加类型,并且不是首次加入缓存
                badge.badge_progress += cond_val;//累加类型，cond_val = 1
            }

            if(badge.badge_progress >= iter->second.badge_cond)
            {//成就项完成
               // badge.badge_status = badge_acquired_no_reward;//将成就项状态置完成未领奖
                badge.badge_status = badge_acquired_no_screen;//将成就项状态置为已获得未查看
                //成就项的统计
                stat_two_param_t stat_data2 = {p_user->uid, 1};
                msg_log(stat_badge_num + badge.badge_id, &stat_data2, sizeof(stat_data2));
            }
            else
            {//成就项仍然未完成
                badge.badge_status = badge_running;

            }

            //告知db更改成就项的状态值以及进度
            //need_update == 2表示不需要更新数据库
            if(need_update != 2 || badge.badge_status != badge_running)
            {//新的成就项或者老的成就项但进度有变化或状态有变化
                 send_badge_to_db(p_user, badge);
            }
            return 0;
        }
    }
    else
    {
        KCRIT_LOG(p_user->uid, "Not found badge %u in g_badge_item_map", badge.badge_id);
        return -1;
    }


    return 0;
}
