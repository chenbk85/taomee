/*
 * =====================================================================================
 *
 *       Filename:  shop.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年02月15日 20时40分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luis, luis@taomee.com
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
#include <libxml/tree.h>
#include <libxml/parser.h>



#include "shop.h"
#include "utility.h"

using namespace std;

c_shop g_shop;

const int c_shop::m_max_flash_time;
const int c_shop::m_flash_interval;
const int c_shop::shop_rarity_num;

c_shop::c_shop(): m_inited(0), m_send_buffer_len(0), m_p_send_header(NULL), m_errno(0), m_userid(0), m_msg_type(0)
{
}

c_shop::~c_shop()
{
    uninit();
}

int c_shop::init()
{
    if (parse_xml_item() != 0)
    {
        ERROR_LOG("parse xml item");
        return -1;
    }


    //解析商店的物品列表
    if (parse_xml_shop() != 0)
    {
        ERROR_LOG("parse xml shop");
        return -1;
    }

    //解析商店各种稀有度物品的数量
    if (parse_xml_shop_num() != 0)
    {
        ERROR_LOG("parse xml shop num");
        return -1;
    }

    //先初始化所有商店物品
    for (map<uint32_t, int>::iterator iter = m_flash_time_map.begin(); iter != m_flash_time_map.end(); ++iter)
    {
        vector<uint32_t> time_vec;
        for (int i = 1; i <= 100; ++i)
        {
            update_shop(i, iter->first);
            time_vec.push_back(0);
        }
        m_shop_update_time_map.insert(pair<uint32_t, vector<uint32_t> >(iter->first, time_vec));
    }
    m_p_send_header = (svr_msg_header_t*)m_send_buffer;

    m_inited = 1;
    return 0;
}

int c_shop::uninit()
{
    if(!m_inited)
    {
        return -1;
    }


    m_inited = 0;

    return 0;
}

int c_shop::parse_xml_shop_num()
{
    const char *shop_num_file = config_get_strval("shop_num_file");
    if (NULL == shop_num_file)
    {
        ERROR_LOG("xml load shop num file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(shop_num_file);
    if (NULL == doc)
    {
        ERROR_LOG("parse xml file failed(%s).", strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if (cur_node == NULL)
    {
        ERROR_LOG("Get root node of xml file failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if (xmlStrcmp(cur_node->name, BAD_CAST "num"))
    {
        ERROR_LOG("Get global_config failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    xmlNodePtr child_node;
    while(cur_node != NULL)
    {
        if (!xmlStrcmp(cur_node->name, BAD_CAST "cat"))
        {
            xmlChar *attr = NULL;

            shop_key_t key = {0};
            if (xmlHasProp(cur_node, (const xmlChar *)"id"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"id");
                key.id = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse shop num failed, no id.");
                xmlFreeDoc(doc);
                return -1;
            }

            child_node = cur_node->xmlChildrenNode;
            child_node = child_node->next;
            while (child_node != NULL)
            {
                if (xmlHasProp(child_node, (const xmlChar *)"id"))
                {
                    attr = xmlGetProp(child_node, (const xmlChar *)"id");
                    key.type = atoi((char *)attr);
                }
                else
                {
                    ERROR_LOG("parse shop num failed, no id.");
                    xmlFreeDoc(doc);
                    return -1;
                }

                shop_num_t num = {0};
                if (xmlHasProp(child_node, (const xmlChar *)"cur_min"))
                {
                    attr = xmlGetProp(child_node, (const xmlChar *)"cur_min");
                    num.cur_level_min = atoi((char *)attr);
                }
                else
                {
                    ERROR_LOG("parse shop num failed, no cur_min.");
                    xmlFreeDoc(doc);
                    return -1;
                }

                if (xmlHasProp(child_node, (const xmlChar *)"cur_max"))
                {
                    attr = xmlGetProp(child_node, (const xmlChar *)"cur_max");
                    num.cur_level_max = atoi((char *)attr);
                }
                else
                {
                    ERROR_LOG("parse shop num failed, no cur_max.");
                    xmlFreeDoc(doc);
                    return -1;
                }

                if (xmlHasProp(child_node, (const xmlChar *)"next_min"))
                {
                    attr = xmlGetProp(child_node, (const xmlChar *)"next_min");
                    num.next_level_min = atoi((char *)attr);
                }
                else
                {
                    ERROR_LOG("parse shop num failed, no next_min.");
                    xmlFreeDoc(doc);
                    return -1;
                }

                if (xmlHasProp(child_node, (const xmlChar *)"next_max"))
                {
                    attr = xmlGetProp(child_node, (const xmlChar *)"next_max");
                    num.next_level_max = atoi((char *)attr);
                }
                else
                {
                    ERROR_LOG("parse shop num failed, no next_max.");
                    xmlFreeDoc(doc);
                    return -1;
                }

                m_shop_num_map.insert(pair<shop_key_t, shop_num_t>(key, num));

                child_node = child_node->next->next;
            }
        }

        cur_node = cur_node->next->next;
    }

    xmlFreeDoc(doc);
    return 0;
}

int c_shop::parse_xml_shop()
{
    const char *shop_file = config_get_strval("shop_file");
    if (NULL == shop_file)
    {
        ERROR_LOG("xml load shop file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(shop_file);
    if (NULL == doc)
    {
        ERROR_LOG("parse xml file failed(%s).", strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if (cur_node == NULL)
    {
        ERROR_LOG("Get root node of xml file failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if (xmlStrcmp(cur_node->name, BAD_CAST "Shops"))
    {
        ERROR_LOG("Get global_config failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    xmlNodePtr child_node;
    while(cur_node != NULL)
    {
        if (!xmlStrcmp(cur_node->name, BAD_CAST "Shop"))
        {
            xmlChar *attr = NULL;

            shop_key_t key = {0};
            if (xmlHasProp(cur_node, (const xmlChar *)"ID"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"ID");
                key.id = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse shop failed, no ID.");
                xmlFreeDoc(doc);
                return -1;
            }

            if (xmlHasProp(cur_node, (const xmlChar *)"Type"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"Type");
                key.type = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse shop failed, no Type.");
                xmlFreeDoc(doc);
                return -1;
            }

            int flash_time = 0;
            if (xmlHasProp(cur_node, (const xmlChar *)"flash_time"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"flash_time");
                flash_time = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse shop failed, no flash_time.");
                xmlFreeDoc(doc);
                return -1;
            }

            m_shop_type_map.insert(pair<uint32_t, int>(key.id, key.type));
            m_flash_time_map.insert(pair<uint32_t, int>(key.id, flash_time));

            vector<uint32_t> item_vec;

            child_node = cur_node->xmlChildrenNode;
            child_node = child_node->next;
            while (child_node != NULL)
            {
                if (!xmlStrcmp(child_node->name, BAD_CAST "Item"))
                {
                    uint32_t item_id = 0;
                    if (xmlHasProp(child_node, (const xmlChar *)"ID"))
                    {
                        attr = xmlGetProp(child_node, (const xmlChar *)"ID");
                        item_id = atoi((char *)attr);
                    }
                    else
                    {
                        ERROR_LOG("parse shop failed, no item:id.");
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    item_vec.push_back(item_id);
                }

                child_node = child_node->next->next;
            }

            m_shop_item_map.insert(pair<uint32_t, vector<uint32_t> >(key.id, item_vec));
        }

        cur_node = cur_node->next->next;
    }

    xmlFreeDoc(doc);
    return 0;
}

void c_shop::create_shop()
{
    time_t timep;
    time(&timep);
    struct tm *p=localtime(&timep); /*取得当地时间*/
    if (!p)
    {
        ERROR_LOG("localtime failed");
        return;
    }
    int seconds = (p->tm_min * 60 + p->tm_sec) % m_flash_interval;
    for (map<uint32_t, int>::iterator iter = m_flash_time_map.begin(); iter != m_flash_time_map.end(); ++iter)
    {
        if (seconds < iter->second || seconds > iter->second + m_max_flash_time)
        {
            continue;
        }

        //有商店需要刷新
        int level = (seconds - iter->second) / (m_max_flash_time / 100) + 1;
        map<uint32_t, std::vector<uint32_t> >::iterator time_iter = m_shop_update_time_map.find(iter->first);
	vector<uint32_t> &time_vec = time_iter->second;
        int last_time = time_vec[level - 1];
        if (timep - last_time >= m_flash_interval)
        {
            update_shop(level, iter->first);
	    vector<uint32_t> &time_vec = time_iter->second;
            time_vec[level - 1] = timep;
        }
    }
}

void c_shop::update_shop(int mon_level, uint32_t shop_id)
{
    INFO_LOG("update shop:%u, level:%u", shop_id, mon_level);
    shop_key_t shop = {shop_id, mon_level};
    m_cur_shop_vec_map.erase(shop);

    map<uint32_t, int>::iterator shop_type_iter = m_shop_type_map.find(shop_id);
    if (shop_type_iter == m_shop_type_map.end())
    {
        ERROR_LOG("cann't find shop:%u type", shop_id);
        return;
    }

    int shop_type = shop_type_iter->second;

    //获得每个稀有度的物品的数量
    int num[shop_rarity_num] = {0};
    int high_level_num[shop_rarity_num] = {0};
    for (int i = 0; i != shop_rarity_num; ++i)
    {
        shop_key_t key = {shop_type, i};

        map<shop_key_t, shop_num_t, c_shop_cmp>::iterator num_iter = m_shop_num_map.find(key);
        if (num_iter == m_shop_num_map.end())
        {
            ERROR_LOG("can't find shop num in g_shop_num_map, type:%u, rarity:%u", key.id, key.type);
            return;
        }

        const shop_num_t &shop_num = num_iter->second;
        if (shop_num.cur_level_min == shop_num.cur_level_max)
        {
            num[i] = shop_num.cur_level_min;
        }
        else
        {
            num[i] = uniform_rand(shop_num.cur_level_min, shop_num.cur_level_max);
        }

        if (shop_num.next_level_min == shop_num.next_level_max)
        {
            high_level_num[i] = shop_num.next_level_min;
        }
        else
        {
            high_level_num[i] = uniform_rand(shop_num.next_level_min, shop_num.next_level_max);
        }
    }
    num[0] += mon_level;

    //随机更新商店物品的位置
    map<uint32_t, vector<uint32_t> >::iterator shop_iter = m_shop_item_map.find(shop_id);
    if (shop_iter == m_shop_item_map.end())
    {
        ERROR_LOG("cann't find shop:%u items", shop_id);
        return;
    }

    vector<uint32_t> &shop_vec = shop_iter->second;
    int item_num = shop_vec.size();
    for (int i = item_num - 1; i != 0; --i)
    {
        int index = uniform_rand(0, i);
        uint32_t temp = shop_vec[index];
        shop_vec[index] = shop_vec[i];
        shop_vec[i] = temp;
    }

    int count[shop_rarity_num] = {0};
    int high_level_count[shop_rarity_num] = {0};
    vector<uint32_t> item_vector;
    for (vector<uint32_t>::iterator item_iter = shop_vec.begin(); item_iter != shop_vec.end(); ++item_iter)
    {
        uint32_t item_id = *item_iter;
        map<uint32_t, item_t>::iterator item_info_iter = m_item_map.find(item_id);
        if (item_info_iter == m_item_map.end())
        {
            ERROR_LOG("item:%u is not in g_item_map.", item_id);
            return;
        }
        int rarity = item_info_iter->second.rarity;

        if (item_info_iter->second.level <= mon_level)
        {
            if (count[rarity] < num[rarity])
            {
                item_vector.push_back(item_id);
                ++count[rarity];
            }
        }
        else if (item_info_iter->second.level <= mon_level + 3)
        {
            if (high_level_count[rarity] < high_level_num[rarity])
            {
                item_vector.push_back(item_id);
                ++high_level_count[rarity];
            }
        }
        else
        {
            //高三级以上的物品，不显示
        }
    }

    INFO_LOG("create %u items", item_vector.size());
    m_cur_shop_vec_map.insert(pair<shop_key_t, vector<uint32_t> >(shop, item_vector));
}

int c_shop::parse_xml_item()
{
    const char *items_file = config_get_strval("items_file");
    if (NULL == items_file)
    {
        ERROR_LOG("xml load items file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(items_file);
    if (NULL == doc)
    {
        ERROR_LOG("parse xml file failed(%s).", strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if (cur_node == NULL)
    {
        ERROR_LOG("Get root node of xml file failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if (xmlStrcmp(cur_node->name, BAD_CAST "items"))
    {
        ERROR_LOG("Get global_config failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    xmlNodePtr child_node;
    while(cur_node != NULL)
    {
        if (!xmlStrcmp(cur_node->name, BAD_CAST "Cat"))
        {
            xmlChar *attr = NULL;

            uint32_t id = 0;
            if (xmlHasProp(cur_node, (const xmlChar *)"ID"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"ID");
                id = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse item failed, no category:id.");
                xmlFreeDoc(doc);
                return -1;
            }
	        DEBUG_LOG("cat id:%u", id);

            child_node = cur_node->xmlChildrenNode;
            child_node = child_node->next;
            while (child_node != NULL)
            {

                if (xmlStrcmp(child_node->name, BAD_CAST "Item"))
                {
                    child_node = child_node->next->next;
                    continue;
                }
                uint32_t item_id = 0;
                if (xmlHasProp(child_node, (const xmlChar *)"ID"))
                {
                    attr = xmlGetProp(child_node, (const xmlChar *)"ID");
                    item_id = atoi((char *)attr);
                }
                else
                {
                    ERROR_LOG("parse item failed, no item:id.");
                    xmlFreeDoc(doc);
                    return -1;
                }
                map<uint32_t, item_t>::iterator iter = m_item_map.find(item_id);
                if (iter != m_item_map.end())
                {
                    ERROR_LOG("item id:%u is exist", item_id);
                    return -1;
                }

                item_t item = {0};
                if (xmlHasProp(child_node, (const xmlChar *)"Price"))
                {
                    attr = xmlGetProp(child_node, (const xmlChar *)"Price");
                    item.price = atoi((char *)attr);
                }
                else
                {
			item.price = 0;
                }

                if (xmlHasProp(child_node, (const xmlChar *)"level"))
                {
                    attr = xmlGetProp(child_node, (const xmlChar *)"level");
                    item.level = atoi((char *)attr);
                }
                else
                {
			item.level = 0;
                }

                if (xmlHasProp(child_node, (const xmlChar *)"happy"))
                {
                    attr = xmlGetProp(child_node, (const xmlChar *)"happy");
                    item.happy = atoi((char *)attr);
                }
                else
                {
                    item.happy = 0;
                }

                if (xmlHasProp(child_node, (const xmlChar *)"health"))
                {
                    attr = xmlGetProp(child_node, (const xmlChar *)"health");
                    item.health = atoi((char *)attr);
                }
                else
                {
                    item.health = 0;
                }

                if (xmlHasProp(child_node, (const xmlChar *)"rarity"))
                {
                    attr = xmlGetProp(child_node, (const xmlChar *)"rarity");
                    item.rarity = atoi((char *)attr);
                }
                else
                {
                    item.rarity = 0;
                }
                if (item.rarity >= shop_rarity_num)
                {
                    ERROR_LOG("item:%u rarity too large", item_id);
                    return -1;
                }

                item.category = id;
                m_item_map.insert(pair<uint32_t, item_t>(item_id, item));

                child_node = child_node->next->next;
            }
        }

        cur_node = cur_node->next->next;
    }

    xmlFreeDoc(doc);
    return 0;
}

int c_shop::get_shop_item(svr_msg_header_t *p_recv_header, int body_len, fdsession_t *fdsess)
{
    m_errno = ERR_NO_ERR;
	m_userid = p_recv_header->user_id;
	m_msg_type = p_recv_header->msg_type;
	m_send_buffer_len = sizeof(svr_msg_header_t);

    shop_key_t *p_shop = (shop_key_t *)p_recv_header->body;
    KINFO_LOG(p_recv_header->user_id, "get shop:%u, level:%u", p_shop->id, p_shop->type);

    uint16_t remain_time = m_max_flash_time;
    map<uint32_t, int>::iterator flash_iter = m_flash_time_map.find(p_shop->id);                  //保存每个店的刷新时间
    if (flash_iter != m_flash_time_map.end())
    {
        remain_time = time(NULL) - flash_iter->second;
    }
    pack(&remain_time, sizeof(remain_time));

    uint16_t count = 0;
    map<shop_key_t, vector<uint32_t> >::iterator iter = m_cur_shop_vec_map.find(*p_shop);
    if (iter == m_cur_shop_vec_map.end())
    {
        pack(&count, sizeof(count));
    }
    else
    {
        vector<uint32_t> &item_vec = iter->second;
        count = item_vec.size();
        pack(&count, sizeof(count));
    	KINFO_LOG(p_recv_header->user_id, "get %u items", count);
        for (vector<uint32_t>::iterator item_iter = item_vec.begin(); item_iter != item_vec.end(); ++item_iter)
        {
            uint32_t item_id = *item_iter;
            pack(&item_id, sizeof(item_id));
    //	    KINFO_LOG(p_recv_header->user_id, "get item:%u", item_id);
        }
    }

    return send_to_client(fdsess, p_recv_header->seq_id, m_msg_type);
}

int c_shop::pack(const void *val, uint32_t val_len)
{
    if(m_send_buffer_len + val_len > MAX_SEND_PKG_SIZE)
    {
        return ERR_SNDBUF_OVERFLOW;
    }

    memcpy(m_send_buffer + m_send_buffer_len, val, val_len);
    m_send_buffer_len += val_len;
    return 0;
}

int c_shop::send_to_client(fdsession_t *fdsess, uint32_t seq_id, uint16_t msg_type)
{
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

