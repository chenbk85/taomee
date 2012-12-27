/*
 * =====================================================================================
 *
 *       Filename:  shop.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年02月15日 20时21分02秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luis, luis@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_SHOP_H_20120215
#define H_SHOP_H_20120215
#include <stdint.h>
#include <map>
#include <set>
#include <vector>

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




class c_shop
{
    public:
        c_shop();
        ~c_shop();
        int init();
        int uninit();

    public:
        //以下均是来自online服务器的协议处理函数
        //50101 online发注册请求信息
        int get_shop_item(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess);

        void create_shop();

    protected:
        int parse_xml_shop();
        int parse_xml_shop_num();
        int parse_xml_item();
        void update_shop(int mon_level, uint32_t shop_id);


        int pack(const void *val, uint32_t val_len);
        int send_to_client(fdsession_t *fdsess, uint32_t seq_id, uint16_t msg_type);

    private:
        typedef struct
        {
            uint16_t cur_level_min;
            uint16_t cur_level_max;
            uint16_t next_level_min;
            uint16_t next_level_max;
        } shop_num_t;

        typedef struct
        {
            uint32_t id;
            uint8_t type;
        } shop_key_t;

	typedef struct
	{
	    uint8_t category;
	    uint16_t price;
	    uint16_t level;
	    uint16_t happy;
	    uint16_t health;
	    uint8_t rarity; //物品的稀有程度，0-3,0表示普通物品
	} item_t;

        class c_shop_cmp
        {
            public:
                bool operator()(const shop_key_t &key_a, const shop_key_t &key_b) const
                {
                    if(key_a.id < key_b.id)
                    {
                        return true;
                    }

                    if(key_a.id == key_b.id)
                    {
                        if(key_a.type < key_b.type)
                        {
                            return true;
                        }
                    }

                    return false;
                }
        };

        uint8_t m_inited;

        char m_send_buffer[MAX_SEND_PKG_SIZE];                      //发送缓冲
        uint32_t m_send_buffer_len;
        svr_msg_header_t *m_p_send_header;                               //指向发送缓冲

        uint32_t m_errno;
        uint32_t m_userid;
        uint16_t m_msg_type;

        static const int m_max_flash_time = 300;
        static const int m_flash_interval = 900;
	static const int shop_rarity_num = 4;

        std::map<uint32_t, int> m_shop_type_map;                   //保存每个商店的类型
        std::map<uint32_t, std::vector<uint32_t> > m_shop_item_map;  //商店的物品列表
        std::map<shop_key_t, shop_num_t, c_shop_cmp> m_shop_num_map;
        std::map<uint32_t, item_t> m_item_map;

        std::map<uint32_t, int> m_flash_time_map;                  //保存每个店的刷新时间
        std::map<uint32_t, std::vector<uint32_t> > m_shop_update_time_map;  //商店的物品列表

        std::map<shop_key_t, std::vector<uint32_t> , c_shop_cmp> m_cur_shop_vec_map;  //产生的每个商店的物品id
};

#endif //H_SHOP_H_20120215
