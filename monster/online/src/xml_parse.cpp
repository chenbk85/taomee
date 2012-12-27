/**
 * =====================================================================================
 *       @file  xml_parse.cpp
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/01/2011 01:54:44 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <map>
#include <set>
#include <vector>
#include <string.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <errno.h>

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
}

#include "online_structure.h"
#include "online_constant.h"
#include "data.h"
#include "xml_parse.h"
#include "utility.h"

using std::map;
using std::set;
using std::pair;
using std::vector;

int parse_xml_formulaes()
{
    g_factory_cost_time_map.clear();
    g_factory_create_map.clear();
    g_factory_cost_map.clear();
    g_factory_unlock_map.clear();

    const char *formulaes_file = config_get_strval("formulaes_file");
    if (NULL == formulaes_file)
    {
        ERROR_LOG("xml load formulaes file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(formulaes_file);
    if (NULL == doc)
    {
        ERROR_LOG("parse xml file %s failed(%s).", formulaes_file, strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if (cur_node == NULL)
    {
        ERROR_LOG("Get root node of xml file failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if (xmlStrcmp(cur_node->name, BAD_CAST "formulaesPool"))
    {
        ERROR_LOG("Get global_config failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    xmlNodePtr child_node;
    xmlNodePtr child_child_node;
    while(cur_node != NULL)
    {
        if (!xmlStrcmp(cur_node->name, BAD_CAST "formula"))
        {
            xmlChar *attr = NULL;

            uint32_t id = 0;
            if (xmlHasProp(cur_node, (const xmlChar *)"id"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"id");
                id = atoi((char *)attr);
                if(g_item_map.find(id) == g_item_map.end())
                {
                    ERROR_LOG("formula id %u not int item map", id);
                    xmlFreeDoc(doc);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse item failed, no category:id.");
                xmlFreeDoc(doc);
                return -1;
            }

            uint32_t cost_time = 0;
            if (xmlHasProp(cur_node, (const xmlChar *)"costTime"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"costTime");
                cost_time = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse item failed, no category:costTime.");
                xmlFreeDoc(doc);
                return -1;
            }

            g_factory_cost_time_map.insert(pair<uint32_t, uint32_t>(id, cost_time));

            child_node = cur_node->xmlChildrenNode;
            child_node = child_node->next;
            while (child_node != NULL)
            {
                if (!xmlStrcmp(child_node->name, BAD_CAST "products"))
                {
                    child_child_node = child_node->xmlChildrenNode;
                    child_child_node = child_child_node->next;

                    vector<uint32_t> item_vec;
                    while (child_child_node != NULL)
                    {
                        if (!xmlStrcmp(child_child_node->name, BAD_CAST "product"))
                        {
                            uint32_t item_id = 0;
                            if (xmlHasProp(child_child_node, (const xmlChar *)"id"))
                            {
                                attr = xmlGetProp(child_child_node, (const xmlChar *)"id");
                                item_id = atoi((char *)attr);
                                if(g_item_map.find(item_id) == g_item_map.end())
                                {
                                    ERROR_LOG("product id %u not int item map", item_id);
                                    xmlFreeDoc(doc);
                                    return -1;
                                }
                                item_vec.push_back(item_id);
                            }
                        }

                        child_child_node = child_child_node->next->next;
                    }

                    g_factory_create_map.insert(pair<uint32_t, std::vector<uint32_t> >(id, item_vec));
                }
                else if (!xmlStrcmp(child_node->name, BAD_CAST "materials"))
                {
                    child_child_node = child_node->xmlChildrenNode;
                    child_child_node = child_child_node->next;

                    vector<factory_cost_t> item_vec;
                    while (child_child_node != NULL)
                    {
                        factory_cost_t cost_item = {0};
                        if (!xmlStrcmp(child_child_node->name, BAD_CAST "material"))
                        {
                            if (xmlHasProp(child_child_node, (const xmlChar *)"id"))
                            {
                                attr = xmlGetProp(child_child_node, (const xmlChar *)"id");
                                cost_item.item_id = atoi((char *)attr);
                                if(g_item_map.find(cost_item.item_id) == g_item_map.end())
                                {
                                    ERROR_LOG("cost item_id %u not int item map", cost_item.item_id);
                                    xmlFreeDoc(doc);
                                    return -1;
                                }
                            }
                            if (xmlHasProp(child_child_node, (const xmlChar *)"num"))
                            {
                                attr = xmlGetProp(child_child_node, (const xmlChar *)"num");
                                cost_item.item_num = atoi((char *)attr);
                            }
                            item_vec.push_back(cost_item);
                        }

                        child_child_node = child_child_node->next->next;
                    }

                    g_factory_cost_map.insert(pair<uint32_t, vector<factory_cost_t> >(id, item_vec));
                }
                else if (!xmlStrcmp(child_node->name, BAD_CAST "condition"))
                {
                    factory_unlock_t unlock_cond;
                    if (xmlHasProp(child_node, (const xmlChar *)"type"))
                    {
                        attr = xmlGetProp(child_node, (const xmlChar *)"type");
                        unlock_cond.unlock_type = atoi((char *)attr);
                    }
                    if (xmlHasProp(child_node, (const xmlChar *)"value"))
                    {
                        attr = xmlGetProp(child_node, (const xmlChar *)"value");
                        unlock_cond.value = atoi((char *)attr);
                    }

                    g_factory_unlock_map.insert(pair<uint32_t, factory_unlock_t>(id, unlock_cond));
                }

                child_node = child_node->next->next;
            }
        }

        cur_node = cur_node->next->next;
    }

    xmlFreeDoc(doc);
    return 0;
}

int parse_xml_items()
{
    g_item_map.clear();

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
                map<uint32_t, item_t>::iterator iter = g_item_map.find(item_id);
                if (iter != g_item_map.end())
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
                if (item.rarity >= SHOP_RARITY_NUM)
                {
                    ERROR_LOG("item:%u rarity too large", item_id);
                    return -1;
                }

                item.category = id;
                g_item_map.insert(pair<uint32_t, item_t>(item_id, item));

                child_node = child_node->next->next;
            }
        }

        cur_node = cur_node->next->next;
    }

    xmlFreeDoc(doc);
    return 0;
}

int parse_xml_shops()
{
    g_npc_num = 0;
    g_item_npc_map.clear();
    g_npc_vec.clear();

    const char *items_file = config_get_strval("shops_file");
    if (NULL == items_file)
    {
        ERROR_LOG("xml load shops file failed.");
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

            uint32_t npc_id = 0;
            if (xmlHasProp(cur_node, (const xmlChar *)"NpcId"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"NpcId");
                npc_id = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse shopfailed, no category:id.");
                xmlFreeDoc(doc);
                return -1;
            }

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

                if(g_item_map.find(item_id) == g_item_map.end())
                {
                    ERROR_LOG("item id:%u not exist", item_id);
                    return -1;
                }

                g_item_npc_map.insert(pair<uint32_t, uint32_t>(item_id, npc_id));

                child_node = child_node->next->next;
            }

            g_npc_vec.push_back(npc_id);
        }

        cur_node = cur_node->next->next;
    }

    xmlFreeDoc(doc);
    g_npc_num = g_npc_vec.size();
    return 0;
}

int parse_xml_pet()
{
    g_max_pet_id = 0;

    g_common_pet.clear();
    g_good_pet.clear();
    g_rare_pet.clear();
    g_vip_pet.clear();

    g_seed_reward.clear();
    g_seed_reward.clear();
    g_seed_reward.clear();

    const char *items_file = config_get_strval("pet_file");
    if (NULL == items_file)
    {
        ERROR_LOG("xml load pet file failed.");
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

    if (xmlStrcmp(cur_node->name, BAD_CAST "pets"))
    {
        ERROR_LOG("Get global_config failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    while(cur_node != NULL)
    {
        if (!xmlStrcmp(cur_node->name, BAD_CAST "pet"))
        {
            xmlChar *attr = NULL;

            uint32_t pet_id = 0;
            if (xmlHasProp(cur_node, (const xmlChar *)"id"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"id");
                pet_id = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse pet failed, no id.");
                xmlFreeDoc(doc);
                return -1;
            }

            if ((int)pet_id > g_max_pet_id)
            {
                g_max_pet_id = pet_id;
            }

            int type = 0;
            if (xmlHasProp(cur_node, (const xmlChar *)"type"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"type");
                type = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse pet failed, no type.");
                xmlFreeDoc(doc);
                return -1;
            }

            int seed1 = 0;
            if (xmlHasProp(cur_node, (const xmlChar *)"seed1"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"seed1");
                seed1 = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse pet failed, no seed1.");
                xmlFreeDoc(doc);
                return -1;
            }

            int seed2 = 0;
            if (xmlHasProp(cur_node, (const xmlChar *)"seed2"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"seed2");
                seed2 = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse pet failed, no seed2.");
                xmlFreeDoc(doc);
                return -1;
            }

            int seed3 = 0;
            if (xmlHasProp(cur_node, (const xmlChar *)"seed3"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"seed3");
                seed3 = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse pet failed, no seed3.");
                xmlFreeDoc(doc);
                return -1;
            }

            int color1 = 0;
            if (xmlHasProp(cur_node, (const xmlChar *)"color1"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"color1");
                color1 = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse pet failed, no color1.");
                xmlFreeDoc(doc);
                return -1;
            }

            int color2 = 0;
            if (xmlHasProp(cur_node, (const xmlChar *)"color2"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"color2");
                color2 = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse pet failed, no color2.");
                xmlFreeDoc(doc);
                return -1;
            }

            int color3 = 0;
            if (xmlHasProp(cur_node, (const xmlChar *)"color3"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"color3");
                color3 = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse pet failed, no color3.");
                xmlFreeDoc(doc);
                return -1;
            }

            map<seed_match_t, int, c_seed_cmp> seed_map;
            seed_match_t seed_match_1 = {seed1, color1};
            seed_map.insert(pair<seed_match_t, int>(seed_match_1, 1));

            seed_match_t seed_match_2 = {seed2, color2};
            map<seed_match_t, int, c_seed_cmp>::iterator iter = seed_map.find(seed_match_2);
            if (iter != seed_map.end())
            {
                iter->second++;
            }
            else
            {
                seed_map.insert(pair<seed_match_t, int>(seed_match_2, 1));
            }

            seed_match_t seed_match_3 = {seed3, color3};
            iter = seed_map.find(seed_match_3);
            if (iter != seed_map.end())
            {
                iter->second++;
            }
            else
            {
                seed_map.insert(pair<seed_match_t, int>(seed_match_3, 1));
            }
#ifdef DEBUG
            for (map<seed_match_t, int, c_seed_cmp>::iterator iter = seed_map.begin(); iter != seed_map.end(); ++iter)
            {
                DEBUG_LOG("pet:%d, seed:%u, color:%u, num:%u", pet_id, iter->first.seed_id, iter->first.color, iter->second);
            }
#endif

            switch (type)
            {
                case COMMON_PET:
                    g_common_pet.insert(pair<uint32_t, map<seed_match_t, int, c_seed_cmp> >(pet_id, seed_map));
                    break;
                case GOOD_PET:
                    g_good_pet.insert(pair<uint32_t, map<seed_match_t, int, c_seed_cmp> >(pet_id, seed_map));
                    break;
                case RARE_PET:
                    g_rare_pet.insert(pair<uint32_t, map<seed_match_t, int, c_seed_cmp> >(pet_id, seed_map));
                    break;
                case VIP_PET:
                    g_vip_pet.insert(pair<uint32_t, map<seed_match_t, int, c_seed_cmp> >(pet_id, seed_map));
                    break;
                default:
                    ERROR_LOG("pet type:%u is invalid.", type);
                    return -1;
            }

        }
        cur_node = cur_node->next->next;
    }


    seed_reward_t reward_seed1 = {90004, 90004, 90004};
    g_seed_reward.push_back(reward_seed1);

    seed_reward_t reward_seed2 = {90004, 90004, 90001};
    g_seed_reward.push_back(reward_seed2);

    seed_reward_t reward_seed3 = {90001, 90001, 90001};
    g_seed_reward.push_back(reward_seed3);


    xmlFreeDoc(doc);
    return 0;
}

int parse_xml_level()
{
    g_level_num = 0;

    const char *xml_file = config_get_strval("level_file");
    if (NULL == xml_file)
    {
        ERROR_LOG("xml load level file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(xml_file);
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

    if (xmlStrcmp(cur_node->name, BAD_CAST "levels"))
    {
        ERROR_LOG("Get global_config failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    g_level_num = 0;
    while(cur_node != NULL)
    {
        xmlChar *attr = NULL;

        if (!xmlStrcmp(cur_node->name, BAD_CAST "item"))
        {
            if (g_level_num > MAX_LEVEL)
            {
                ERROR_LOG("level num:%u is larger than max:%u.", g_level_num, MAX_LEVEL);
                return -1;
            }
            if (xmlHasProp(cur_node, (const xmlChar *)"exp"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"exp");
                g_level[g_level_num] = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse hall failed, no level.");
                xmlFreeDoc(doc);
                return -1;
            }

            ++g_level_num;
        }

        cur_node = cur_node->next->next;
    }

    DEBUG_LOG("g_level_num =%d", g_level_num);
    xmlFreeDoc(doc);
    return 0;
}

int parse_xml_game()
{

    g_game_level_map.clear();
    g_game_reward_map.clear();
    g_game_map.clear();



    const char *xml_file = config_get_strval("game_file");
    if (NULL == xml_file)
    {
        ERROR_LOG("xml load game file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(xml_file);
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

    if (xmlStrcmp(cur_node->name, BAD_CAST "games"))
    {
        ERROR_LOG("Get global_config failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    xmlNodePtr child_node;
    xmlNodePtr level_node;
    xmlNodePtr item_node;
    while(cur_node != NULL)
    {
        xmlChar *attr = NULL;

        if (!xmlStrcmp(cur_node->name, BAD_CAST "game"))
        {
            uint32_t game_id = 0;
            char key_buf[16] = {0};
            uint32_t key = 0;
            if (xmlHasProp(cur_node, (const xmlChar *)"id"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"id");
                game_id = atoi((char *)attr);
            }
            else
            {
                ERROR_LOG("parse game failed, no id.");
                xmlFreeDoc(doc);
                return -1;
            }

            level_node = cur_node->xmlChildrenNode;
            level_node = level_node->next;
            uint32_t level_count = 0;
            while (level_node != NULL)
            {
                if(!xmlStrcmp(level_node->name, BAD_CAST"level"))
                {
                    map<int, game_item_t> game_reward_map;
                    game_level_item_t game_level;
                    if(xmlHasProp(level_node, (const xmlChar*)"id"))
                    {
                        attr = xmlGetProp(level_node, (const xmlChar*)"id");
                        game_level.level_id = atoi((char*)attr);
                        sprintf(key_buf, "%u%u", game_id, game_level.level_id);
                        key = atoi(key_buf);
                    }
                    else
                    {
                        ERROR_LOG("parse game level failed, no id.");
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    if(xmlHasProp(level_node, (const xmlChar*)"one_star_score"))
                    {
                        attr = xmlGetProp(level_node, (const xmlChar*)"one_star_score");
                        game_level.one_star_score = atoi((char*)attr);
                    }
                    else
                    {
                        ERROR_LOG("parse game level failed, no one_star_score.");
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    if(xmlHasProp(level_node, (const xmlChar*)"two_star_score"))
                    {
                        attr = xmlGetProp(level_node, (const xmlChar*)"two_star_score");
                        game_level.two_star_score = atoi((char*)attr);
                    }
                    else
                    {
                        ERROR_LOG("parse game level failed, no two_star_score.");
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    if(xmlHasProp(level_node, (const xmlChar*)"three_star_score"))
                    {
                        attr = xmlGetProp(level_node, (const xmlChar*)"three_star_score");
                        game_level.three_star_score = atoi((char*)attr);
                    }
                    else
                    {
                        ERROR_LOG("parse game level failed, no three_star_score.");
                        xmlFreeDoc(doc);
                        return -1;
                    }
                    if(xmlHasProp(level_node, (const xmlChar*)"finish_score"))
                    {
                        attr = xmlGetProp(level_node, (const xmlChar*)"finish_score");
                        game_level.finished_score = atoi((char*)attr);
                    }
                    else
                    {
                        ERROR_LOG("parse game level failed, no finish_score.");
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    if(xmlHasProp(level_node, (const xmlChar*)"unlock_cond"))
                    {
                        attr = xmlGetProp(level_node, (const xmlChar*)"unlock_cond");
                        game_level.unlock_cond = atoi((char*)attr);
                    }
                    else
                    {
                        ERROR_LOG("parse game level failed, no unlock_cond.");
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    if(xmlHasProp(level_node, (const xmlChar*)"reward_item_score"))
                    {
                        attr = xmlGetProp(level_node, (const xmlChar*)"reward_item_score");
                        game_level.reward_item_score = atoi((char*)attr);
                    }
                    else
                    {
                        ERROR_LOG("parse game level failed, no reward_item_score.");
                        xmlFreeDoc(doc);
                        return -1;
                    }


                    level_count++;
                    g_game_level_map.insert(pair<uint32_t, game_level_item_t>(key, game_level));

                    child_node = level_node->xmlChildrenNode;
                    child_node = child_node->next;
                    while(child_node != NULL)
                    {
                        if (!xmlStrcmp(child_node->name, BAD_CAST "field"))
                        {
                            int score = 0;
                            if (xmlHasProp(child_node, (const xmlChar *)"score"))
                            {
                                attr = xmlGetProp(child_node, (const xmlChar *)"score");
                                score = atoi((char *)attr);
                            }
                            else
                            {
                                ERROR_LOG("parse field score failed, no score.");
                                xmlFreeDoc(doc);
                                return -1;
                            }

                            game_item_t game_item = {0};
                            if (xmlHasProp(child_node, (const xmlChar *)"num"))
                            {
                                attr = xmlGetProp(child_node, (const xmlChar *)"num");
                                game_item.num = atoi((char *)attr);
                            }
                            else
                            {
                                ERROR_LOG("parse field num failed, no num.");
                                xmlFreeDoc(doc);
                                return -1;
                            }

                            item_node = child_node->xmlChildrenNode;
                            item_node = item_node->next;
                            while (item_node != NULL)
                            {
                                if (!xmlStrcmp(item_node->name, BAD_CAST "item"))
                                {
                                    if (xmlHasProp(item_node, (const xmlChar *)"id"))
                                    {
                                        attr = xmlGetProp(item_node, (const xmlChar *)"id");
                                        game_item.item_id[game_item.item_num] = atoi((char *)attr);
                                        ++game_item.item_num;
                                    }
                                    else
                                    {
                                        ERROR_LOG("parse field id failed.");
                                        xmlFreeDoc(doc);
                                        return -1;
                                    }
                                }
                                item_node = item_node->next->next;
                            }

                            game_reward_map.insert(pair<int, game_item_t>(score, game_item));
                        }
                        child_node = child_node->next->next;
                    }

                    g_game_reward_map.insert(pair<uint32_t, map<int, game_item_t> >(key, game_reward_map));
                }

                level_node = level_node->next->next;
            }
            g_game_map.insert(pair<uint32_t, uint32_t>(game_id, level_count));
        }
        cur_node = cur_node->next->next;
    }

    xmlFreeDoc(doc);
    return 0;
}

int parse_xml_badge()
{
    g_badge_item_map.clear();
    const char *xml_file = config_get_strval("badge_file");
    if(NULL == xml_file)
    {
        ERROR_LOG("xml load badge file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(xml_file);
    if(NULL == doc)
    {
        ERROR_LOG("parse xml file failed(%s).", strerror(errno));
        return -1;
    }
    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if(cur_node == NULL)
    {
        ERROR_LOG("Get root node of file %s failed(%s).", xml_file, strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if(xmlStrcmp(cur_node->name, BAD_CAST"achievements_pool"))
    {
        ERROR_LOG("Root element of file %s is not achievements_pool", xml_file);
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    while(cur_node != NULL)
    {
        xmlChar *attr = NULL;
        uint32_t badge_id = 0;
        badge_xml_info_t badge_info;
        if(!xmlStrcmp(cur_node->name, BAD_CAST "achievement"))
        {
            if(xmlHasProp(cur_node, (const xmlChar*)"id"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"id");
                badge_id = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include id element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"condValue"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"condValue");
                badge_info.badge_cond = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include condValue= element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"award"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"award");
                badge_info.badge_reward_id = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include award element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"progress_type"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"progress_type");
                if(strcmp((char*)attr, "TOP") == 0)
                {
                    badge_info.badge_progress_type = BADGE_PROGRESS_TOP;
                }
                else if(strcmp((char*)attr, "LATEST") == 0)
                {
                    badge_info.badge_progress_type = BADGE_PROGRESS_LATEST;
                }
                else
                {
                    ERROR_LOG("unknown badge_progress_type:%s", (char*)attr);
                    xmlFreeDoc(doc);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("file %s not include award element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }
            if(xmlHasProp(cur_node, (const xmlChar*)"type"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"type");
                if(strcmp((char*)attr, "SET") == 0)
                {
                    badge_info.badge_type = BADGE_TYPE_SET;
                }
                else if(strcmp((char*)attr, "SUM") == 0)
                {
                    badge_info.badge_type = BADGE_TYPE_SUM;
                }
                else
                {
                    ERROR_LOG("unknown badge_type:%s", (char*)attr);
                    xmlFreeDoc(doc);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("file %s not include award element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            KDEBUG_LOG(0, "badge_id:%u, condvalue:%u, award:%u, type:%u", badge_id, badge_info.badge_cond, badge_info.badge_reward_id, badge_info.badge_type);
            g_badge_item_map.insert(pair<uint32_t, badge_xml_info_t>(badge_id, badge_info));
        }

        cur_node = cur_node->next->next;
    }

    return 0;
}

int parse_xml_maintain()
{
    g_maintain_map.clear();

    const char *xml_file = config_get_strval("maintain_reward");
    if(NULL == xml_file)
    {
        ERROR_LOG("xml load maintain_reward failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(xml_file);
    if(NULL == doc)
    {
        ERROR_LOG("parse xml file failed(%s).", strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if(cur_node == NULL)
    {
        ERROR_LOG("Get root node of file %s failed(%s).", xml_file, strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if(xmlStrcmp(cur_node->name, BAD_CAST"plantation_maintain"))
    {
        ERROR_LOG("Root element of file %s is not plantation", xml_file);
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    while(cur_node != NULL)
    {
        xmlChar *attr = NULL;
        uint8_t mt_id = 0;
        maintain_type_t mt_type;
        if(!xmlStrcmp(cur_node->name, BAD_CAST "maintain"))
        {
            if(xmlHasProp(cur_node, (const xmlChar*)"id"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"id");
                mt_id = atoi((char*)attr);
                mt_type.type = mt_id;
            }
            else
            {
                ERROR_LOG("file %s not include id element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"reward_coins"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"reward_coins");
                mt_type.reward_coins = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include reward_coins element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"reward_growth"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"reward_growth");
                mt_type.reward_growth = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include reward_growth element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"reward_happy"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"reward_happy");
                mt_type.reward_happy = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include reward_happy element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"reward_exp"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"reward_exp");
                mt_type.reward_exp = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include reward_exp element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"rate"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"rate");
                mt_type.rate = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include rate element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            g_maintain_map.insert(pair<uint8_t, maintain_type_t>(mt_id, mt_type));
        }
        cur_node = cur_node->next->next;
    }

    xmlFreeDoc(doc);

    return 0;
}

int parse_xml_interactive()
{

    g_interactive_reward_map.clear();

    const char *xml_file = config_get_strval("interactive_file");
    if(NULL == xml_file)
    {
        ERROR_LOG("xml load interactive file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(xml_file);
    if(NULL == doc)
    {
        ERROR_LOG("parse xml file failed(%s).", strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if(cur_node == NULL)
    {
        ERROR_LOG("Get root node of file %s failed(%s).", xml_file, strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if(xmlStrcmp(cur_node->name, BAD_CAST"interactive_rewards"))
    {
        ERROR_LOG("Root element of file %s is not interactive_rewards", xml_file);
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    while(cur_node != NULL)
    {
        xmlChar *attr = NULL;
        uint32_t interactive_id = 0;
        interactive_rewards_t rewards;
        if(!xmlStrcmp(cur_node->name, BAD_CAST "interactive"))
        {
            if(xmlHasProp(cur_node, (const xmlChar*)"id"))
            {
                  attr = xmlGetProp(cur_node, (const xmlChar*)"id");
                  interactive_id = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include id element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"min_reward_gudong"))
            {
                  attr = xmlGetProp(cur_node, (const xmlChar*)"min_reward_gudong");
                  rewards.min_rewards = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include min_reward_gudong element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"max_reward_gudong"))
            {
                  attr = xmlGetProp(cur_node, (const xmlChar*)"max_reward_gudong");
                  rewards.max_rewards = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include max_reward_gudong element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            g_interactive_reward_map.insert(pair<uint32_t, interactive_rewards_t>(interactive_id, rewards));
        }
        cur_node = cur_node->next->next;
    }

    xmlFreeDoc(doc);

    return 0;
}

int parse_xml_system()
{
    const char *xml_file = config_get_strval("system_file");
    if (NULL == xml_file)
    {
        ERROR_LOG("xml load system file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(xml_file);
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

    if (xmlStrcmp(cur_node->name, BAD_CAST "system"))
    {
        ERROR_LOG("Get global_config failed(%s).", strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    while(cur_node != NULL)
    {
        xmlChar *attr = NULL;

        if (!xmlStrcmp(cur_node->name, BAD_CAST "plant_min_time"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_plant_time = atoi((char *)attr);
                if (g_plant_time <= 0)
                {
                    ERROR_LOG("g_plant_time:%d", g_plant_time);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no plant_min_time.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "plant_maintain_time"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_maintain_plant_time = atoi((char *)attr);
                if (g_maintain_plant_time <= 0)
                {
                    ERROR_LOG("g_maintain_plant_time:%d", g_maintain_plant_time);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no g_maintain_plant_time.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "plant_max_exp"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_max_exp_in_plant = atoi((char *)attr);
                if (g_max_exp_in_plant <= 0)
                {
                    ERROR_LOG("g_max_exp_in_plant:%d", g_max_exp_in_plant);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no g_max_exp_in_plant.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "extra_plant_growth_value"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_extra_growth = atoi((char *)attr);
                if (g_extra_growth <= 0 || g_extra_growth >= MATURITY_GROWTH_VALUE)
                {
                    ERROR_LOG("g_extra_growth:%d", g_extra_growth);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no extra plant growth value.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "shop_update_time"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_shop_update_time = atoi((char *)attr);
                if (g_shop_update_time <= 0)
                {
                    ERROR_LOG("g_shop_update_time:%d", g_shop_update_time);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no shop update time.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "street_normal_user_num"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_street_normal_num = atoi((char *)attr);
                if (g_street_normal_num <= 0 || g_street_normal_num > 100)
                {
                    ERROR_LOG("g_street_normal_num:%d", g_street_normal_num);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no street_noraml_user_num.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "street_vip_user_num"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_street_vip_num = atoi((char *)attr);
                if (g_street_vip_num <= 0 || g_street_vip_num > 100)
                {
                    ERROR_LOG("g_street_normal_num:%d", g_street_vip_num);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no street_vip_user_num.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "health_desc_offline"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_health_desc_offline = atoi((char *)attr);
                if (g_health_desc_offline <= 0)
                {
                    ERROR_LOG("g_health_desc_offline:%d", g_health_desc_offline);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no g_health_desc_offline.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "health_desc_high"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_health_desc_high = atoi((char *)attr);
                if (g_health_desc_high <= 0)
                {
                    ERROR_LOG("g_health_desc_high:%d", g_health_desc_high);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no g_health_desc_high.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "health_desc_middle"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_health_desc_middle = atoi((char *)attr);
                if (g_health_desc_middle <= g_health_desc_high)
                {
                    ERROR_LOG("g_health_desc_middle:%d", g_health_desc_middle);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no g_health_desc_middle.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "health_desc_slow"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_health_desc_slow = atoi((char *)attr);
                if (g_health_desc_slow <= g_health_desc_middle)
                {
                    ERROR_LOG("g_health_desc_slow:%d", g_health_desc_slow);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no g_health_desc_slow.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "happy_desc_time"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_happy_desc_time = atoi((char *)attr);
                if (g_happy_desc_time <= 0)
                {
                    ERROR_LOG("g_happy_desc_time:%d", g_happy_desc_time);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no g_happy_desc_time.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "happy_growth_time"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_happy_growth_time = atoi((char *)attr);
                if (g_happy_growth_time <= 0)
                {
                    ERROR_LOG("g_happy_growth_time:%d", g_happy_growth_time);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no g_happy_growth_time.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "unhappy_growth_time"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_unhappy_growth_time = atoi((char *)attr);
                if (g_unhappy_growth_time <= 0)
                {
                    ERROR_LOG("g_unhappy_growth_time:%d", g_unhappy_growth_time);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no g_unhappy_growth_time.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "max_game_score"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_max_game_score = atoi((char *)attr);
                if (g_max_game_score <= 0)
                {
                    ERROR_LOG("g_max_game_score:%d", g_max_game_score);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no g_max_game_score.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "max_game_item"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_max_game_item = atoi((char *)attr);
                if (g_max_game_item <= 0)
                {
                    ERROR_LOG("g_max_game_item:%d", g_max_game_item);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no g_max_game_item.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "max_stuff_exp_day"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_max_day_stuff_exp = atoi((char *)attr);
                if (g_max_day_stuff_exp <= 0)
                {
                    ERROR_LOG("g_max_day_stuff_exp:%d", g_max_day_stuff_exp);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no g_max_day_stuff_exp.");
                xmlFreeDoc(doc);
                return -1;
            }
        }
        else if (!xmlStrcmp(cur_node->name, BAD_CAST "max_puzzle_exp_day"))
        {
            if (xmlHasProp(cur_node, (const xmlChar *)"value"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar *)"value");
                g_max_day_puzzle_exp = atoi((char *)attr);
                if (g_max_day_puzzle_exp <= 0)
                {
                    ERROR_LOG("g_max_day_puzzle_exp:%d", g_max_day_puzzle_exp);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("parse system failed, no g_max_day_puzzle_exp.");
                xmlFreeDoc(doc);
                return -1;
            }
        }

        cur_node = cur_node->next->next;
    }

    xmlFreeDoc(doc);
    return 0;
}

int parse_xml_pet_series()
{

    g_pet_series_map.clear();
    const char *xml_file = config_get_strval("pet_series");
    if(NULL == xml_file)
    {
        ERROR_LOG("xml load pet_series file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(xml_file);
    if(NULL == doc)
    {
        ERROR_LOG("parse xml file failed(%s).", strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if(cur_node == NULL)
    {
        ERROR_LOG("Get root node of file %s failed(%s).", xml_file, strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if(xmlStrcmp(cur_node->name, BAD_CAST"petSeries"))
    {
        ERROR_LOG("Root element of file %s is not petSeries", xml_file);
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    xmlNodePtr child_node;
    cur_node = cur_node->next;
    while(cur_node != NULL)
    {
        xmlChar *attr = NULL;
        uint32_t series_id = 0;

        if(!xmlStrcmp(cur_node->name, BAD_CAST "Series"))
        {
            if(xmlHasProp(cur_node, (const xmlChar*)"ID"))
            {
                  attr = xmlGetProp(cur_node, (const xmlChar*)"ID");
                  series_id = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include id element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }
        }

        child_node = cur_node->xmlChildrenNode;
        child_node = child_node->next;
        while(child_node != NULL)
        {
            xmlChar *child_attr = NULL;
            uint32_t pet_id = 0;
            if(!xmlStrcmp(child_node->name, BAD_CAST "pet"))
            {
                if(xmlHasProp(child_node, (const xmlChar*)"id"))
                {
                      child_attr = xmlGetProp(child_node, (const xmlChar*)"id");
                      pet_id = atoi((char*)child_attr);
                }
                else
                {
                    ERROR_LOG("file %s not include id element", xml_file);
                    xmlFreeDoc(doc);
                    return -1;
                }
             }
           g_pet_series_map.insert(pair<uint32_t, uint32_t>(pet_id, series_id));

            child_node = child_node->next->next;
        }


        cur_node = cur_node->next->next;
    }


    return 0;
}

int parse_xml_bobo()
{
    const char *xml_file = config_get_strval("bobo_file");
    if(NULL == xml_file)
    {
        ERROR_LOG("xml load pet_series file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(xml_file);
    if(NULL == doc)
    {
        ERROR_LOG("parse xml file failed(%s).", strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if(cur_node == NULL)
    {
        ERROR_LOG("Get root node of file %s failed(%s).", xml_file, strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if(xmlStrcmp(cur_node->name, BAD_CAST"newspaperPool"))
    {
        ERROR_LOG("Root element of file %s is not newspaperPool", xml_file);
        xmlFreeDoc(doc);
        return -1;
    }

    xmlChar *attr = NULL;

    if(xmlHasProp(cur_node, (const xmlChar*)"currentIssueId"))
    {
        attr = xmlGetProp(cur_node, (const xmlChar*)"currentIssueId");
        g_current_paper_term = atoi((char*)attr);
    }
    else
    {
        ERROR_LOG("file %s not include currentIssueId element", xml_file);
        xmlFreeDoc(doc);
        return -1;
    }

    if(xmlHasProp(cur_node, (const xmlChar*)"rewardsMin"))
    {
        attr = xmlGetProp(cur_node, (const xmlChar*)"rewardsMin");
        g_bobo_reward_min = atoi((char*)attr);
    }
    else
    {
        ERROR_LOG("file %s not include rewardsMin element", xml_file);
        xmlFreeDoc(doc);
        return -1;
    }
    if(xmlHasProp(cur_node, (const xmlChar*)"rewardsMax"))
    {
        attr = xmlGetProp(cur_node, (const xmlChar*)"rewardsMax");
        g_bobo_reward_max = atoi((char*)attr);
    }
    else
    {
        ERROR_LOG("file %s not include rewardsMin element", xml_file);
        xmlFreeDoc(doc);
        return -1;
    }



    return 0;
}


int parse_xml_game_change()
{

    g_game_change_period_map.clear();
    g_game_change_stuff_map.clear();

    const char *xml_file = config_get_strval("game_change");
    if(NULL == xml_file)
    {
        ERROR_LOG("xml load game_change failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(xml_file);
    if(NULL == doc)
    {
        ERROR_LOG("parse xml file failed(%s).", strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if(cur_node == NULL)
    {
        ERROR_LOG("Get root node of file %s failed(%s).", xml_file, strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if(xmlStrcmp(cur_node->name, BAD_CAST"game2"))
    {
        ERROR_LOG("Root element of file %s is not game2", xml_file);
        xmlFreeDoc(doc);
        return -1;
    }

    xmlNodePtr child_node;
    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    while(cur_node != NULL)
    {
        xmlChar *attr = NULL;
        uint32_t game_id = 0;
        uint32_t period_id = 0;
        if(!xmlStrcmp(cur_node->name, BAD_CAST "game"))
        {
            if(xmlHasProp(cur_node, (const xmlChar*)"id"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"id");
                game_id = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include id element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"period_id"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"period_id");
                period_id = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include period_id element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            g_game_change_period_map.insert(pair<uint32_t, uint32_t>(game_id, period_id));

            child_node = cur_node->xmlChildrenNode;
            child_node = child_node->next;
            std::map<uint32_t, uint8_t> stuff_map;
            while(child_node != NULL)
            {
                xmlChar *child_attr = NULL;
                uint32_t stuff_id = 0;
                uint8_t repeat = 0;//是否可重复兑换
                if(!xmlStrcmp(child_node->name, BAD_CAST "stuff"))
                {
                    if(xmlHasProp(child_node, (const xmlChar*)"id"))
                    {
                        child_attr = xmlGetProp(child_node, (const xmlChar*)"id");
                        stuff_id = atoi((char*)child_attr);
                    }
                    else
                    {
                        ERROR_LOG("file %s not include id element", xml_file);
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    if(xmlHasProp(child_node, (const xmlChar*)"repeat"))
                    {
                        child_attr = xmlGetProp(child_node, (const xmlChar*)"repeat");
                        repeat = atoi((char*)child_attr);
                    }
                    else
                    {
                        ERROR_LOG("file %s not include repeat element", xml_file);
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    stuff_map.insert(std::pair<uint32_t, uint8_t>(stuff_id, repeat));
                }
                child_node = child_node->next->next;
            }
            g_game_change_stuff_map.insert(pair<uint32_t, std::map<uint32_t, uint8_t> >(game_id, stuff_map));
        }
        cur_node = cur_node->next->next;
    }

    xmlFreeDoc(doc);
    return 0;
}

int parse_xml_task()
{
    g_task_map.clear();
    const char *xml_file = config_get_strval("task_file");
    if(NULL == xml_file)
    {
        ERROR_LOG("xml load task_file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(xml_file);
    if(NULL == doc)
    {
        ERROR_LOG("parse xml file failed(%s).", strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if(cur_node == NULL)
    {
        ERROR_LOG("Get root node of file %s failed(%s).", xml_file, strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if(xmlStrcmp(cur_node->name, BAD_CAST"missionesPool"))
    {
        ERROR_LOG("Root element of file %s is not missionesPool", xml_file);
        xmlFreeDoc(doc);
        return -1;
    }

    xmlNodePtr child_node;
    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    while(cur_node != NULL)
    {
        xmlChar *attr = NULL;
        uint32_t task_id = 0;
        task_xml_t a_task;
        if(!xmlStrcmp(cur_node->name, BAD_CAST "mission"))
        {
            if(xmlHasProp(cur_node, (const xmlChar*)"id"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"id");
                task_id = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include id element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            a_task.task_id = task_id;

            if(xmlHasProp(cur_node, (const xmlChar*)"lvl"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"lvl");
                a_task.lvl = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include lvl element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"pre_mission"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"pre_mission");
                a_task.pre_mission = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include pre_mission element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"reward_cycle"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"reward_cycle");
                a_task.reward_cycle = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include reward_cycle element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(a_task.reward_cycle <= 0 || a_task.reward_cycle >= 65535)
            {
                ERROR_LOG("reward_cycle not valid %u must between 0 and 65535", a_task.reward_cycle);
                xmlFreeDoc(doc);
                return -1;
            }

            child_node = cur_node->xmlChildrenNode;
            child_node = child_node->next;
            while(child_node != NULL)
            {
                xmlChar *child_attr = NULL;
                task_reward_t task_reward;
                if(!xmlStrcmp(child_node->name, BAD_CAST "reward"))
                {
                    if(xmlHasProp(child_node, (const xmlChar*)"type"))
                    {
                        child_attr = xmlGetProp(child_node, (const xmlChar*)"type");
                        task_reward.reward_id = atoi((char*)child_attr);
                    }
                    else
                    {
                        ERROR_LOG("file %s not include type element", xml_file);
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    if(xmlHasProp(child_node, (const xmlChar*)"value"))
                    {
                        child_attr = xmlGetProp(child_node, (const xmlChar*)"value");
                        task_reward.reward_num = atoi((char*)child_attr);
                    }
                    else
                    {
                        ERROR_LOG("file %s not include value element", xml_file);
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    a_task.task_reward_vec.push_back(task_reward);
                }
                child_node = child_node->next->next;
            }

            DEBUG_LOG("a-task_reward_numL:%u", a_task.task_reward_vec.size());
            g_task_map.insert(pair<uint32_t, task_xml_t>(task_id, a_task));
        }
        cur_node = cur_node->next->next;
    }

    xmlFreeDoc(doc);

    return 0;
}

int parse_xml_donate()
{
    g_donate_map.clear();
    const char *xml_file = config_get_strval("donate_file");
    if(NULL == xml_file)
    {
        ERROR_LOG("xml load donate_file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(xml_file);
    if(NULL == doc)
    {
        ERROR_LOG("parse xml file failed(%s).", strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if(cur_node == NULL)
    {
        ERROR_LOG("Get root node of file %s failed(%s).", xml_file, strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if(xmlStrcmp(cur_node->name, BAD_CAST"solicitCntPool"))
    {
        ERROR_LOG("Root element of file %s is not solicitCntPool", xml_file);
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    while(cur_node != NULL)
    {
        xmlChar *attr = NULL;
        uint32_t donate_id = 0;
        donate_info_t a_donate;
        if(!xmlStrcmp(cur_node->name, BAD_CAST "solicitCnt"))
        {
            if(xmlHasProp(cur_node, (const xmlChar*)"id"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"id");
                donate_id = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include id element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            a_donate.donate_id = donate_id;

            if(xmlHasProp(cur_node, (const xmlChar*)"cntCount"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"cntCount");
                a_donate.donate_total = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include cntCount element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"helped_pet"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"helped_pet");
                a_donate.helped_pet = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include helped_pet element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"restrict_per_person"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"restrict_per_person");
                a_donate.donate_limit = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include restrict_per_person element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            if(xmlHasProp(cur_node, (const xmlChar*)"rewardId"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"rewardId");
                a_donate.reward_id = atoi((char*)attr);
                if(g_item_map.find(a_donate.reward_id) == g_item_map.end())
                {
                    ERROR_LOG("reward_id %u not in item map", a_donate.reward_id);
                    xmlFreeDoc(doc);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("file %s not include rewardId element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }
            g_donate_map.insert(pair<uint32_t, donate_info_t>(donate_id, a_donate));
        }
        cur_node = cur_node->next->next;
    }

    xmlFreeDoc(doc);

    return 0;
}

int parse_xml_museum()
{

    g_museum_level_map.clear();
    g_museum_map.clear();
    const char *xml_file = config_get_strval("museum_file");
    if(NULL == xml_file)
    {
        ERROR_LOG("xml load museum_file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(xml_file);
    if(NULL == doc)
    {
        ERROR_LOG("parse xml file failed(%s).", strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if(cur_node == NULL)
    {
        ERROR_LOG("Get root node of file %s failed(%s).", xml_file, strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if(xmlStrcmp(cur_node->name, BAD_CAST"MuseumConfig"))
    {
        ERROR_LOG("Root element of file %s is not museumconfig", xml_file);
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    xmlNodePtr child_node;
    while(cur_node != NULL)
    {
        xmlChar *attr = NULL;
        uint32_t museum_id = 0;
        level_item_t level_item;
        if(!xmlStrcmp(cur_node->name, BAD_CAST "Branche"))
        {
            if(xmlHasProp(cur_node, (const xmlChar*)"ID"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"ID");
                museum_id = atoi((char *)attr);
                level_item.museum_id = museum_id;
            }
            else
            {
                ERROR_LOG("file %s not include id element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }
            uint8_t max_level_id = 0;
            child_node = cur_node->xmlChildrenNode;
            child_node = child_node->next;
            while(child_node != NULL)
            {
                if(!xmlStrcmp(child_node->name, BAD_CAST "reject"))
                {
                    if(xmlHasProp(child_node, (const xmlChar*)"level"))
                    {
                        attr = xmlGetProp(child_node, (const xmlChar*)"level");
                        level_item.level_id = atoi((char*)attr);
                        if(level_item.level_id > max_level_id)
                        {
                            max_level_id = level_item.level_id;
                        }
                    }
                    else
                    {
                        ERROR_LOG("file %s not include level element", xml_file);
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    if(xmlHasProp(child_node, (const xmlChar*)"questionNum"))
                    {
                        attr = xmlGetProp(child_node, (const xmlChar*)"questionNum");
                        level_item.question_num = atoi((char*)attr);
                    }
                    else
                    {
                        ERROR_LOG("file %s not include questionNum element", xml_file);
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    if(xmlHasProp(child_node, (const xmlChar*)"reward_id"))
                    {
                        attr = xmlGetProp(child_node, (const xmlChar*)"reward_id");
                        level_item.reward_id = atoi((char*)attr);
                    }
                    else
                    {
                        ERROR_LOG("file %s not include restrict_per_person element", xml_file);
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    if(xmlHasProp(child_node, (const xmlChar*)"reward_num"))
                    {
                        attr = xmlGetProp(child_node, (const xmlChar*)"reward_num");
                        level_item.reward_num = atoi((char*)attr);
                    }
                    else
                    {
                        ERROR_LOG("file %s not include reward_num element", xml_file);
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    uint32_t key = level_item.museum_id * 100 + level_item.level_id;
                    g_museum_level_map.insert(pair<uint32_t, level_item_t>(key, level_item));
                }//endif
                child_node = child_node->next->next;
            }//while
            g_museum_map.insert(pair<uint32_t, uint8_t>(museum_id, max_level_id));
        }//endif
        cur_node = cur_node->next->next;
    }//endwhile

    xmlFreeDoc(doc);
    return 0;
}

int parse_xml_user_name()
{

    g_female_name_vec.clear();
    g_male_name_vec.clear();
    const char *xml_file = config_get_strval("name_file");
    if(NULL == xml_file)
    {
        ERROR_LOG("xml load name_file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(xml_file);
    if(NULL == doc)
    {
        ERROR_LOG("parse xml file failed(%s).", strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if(cur_node == NULL)
    {
        ERROR_LOG("Get root node of file %s failed(%s).", xml_file, strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if(xmlStrcmp(cur_node->name, BAD_CAST"name_set"))
    {
        ERROR_LOG("Root element of file %s is not name_set", xml_file);
        xmlFreeDoc(doc);
        return -1;
    }

    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    xmlNodePtr child_node;
    while(cur_node != NULL)
    {
        xmlChar *attr = NULL;
        if(!xmlStrcmp(cur_node->name, BAD_CAST "male"))
        {//男生姓名
            child_node = cur_node->xmlChildrenNode;
            child_node = child_node->next;
            while(child_node != NULL)
            {
                user_name_t name = {{0}};
                if(!xmlStrcmp(child_node->name, BAD_CAST "name"))
                {
                    if(xmlHasProp(child_node, (const xmlChar*)"des"))
                    {
                        attr = xmlGetProp(child_node, (const xmlChar*)"des");
                        memcpy(name.name, (char*)attr, strlen((char*)attr));
                    }
                    else
                    {
                        ERROR_LOG("file %s not include level element", xml_file);
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    g_male_name_vec.push_back(name);
                }//endif
                child_node = child_node->next->next;
            }//while
        }//endif
        else if(!xmlStrcmp(cur_node->name, BAD_CAST "female"))
        {//女性姓名
            child_node = cur_node->xmlChildrenNode;
            child_node = child_node->next;
            while(child_node != NULL)
            {
                user_name_t name = {{0}};
                if(!xmlStrcmp(child_node->name, BAD_CAST "name"))
                {
                    if(xmlHasProp(child_node, (const xmlChar*)"des"))
                    {
                        attr = xmlGetProp(child_node, (const xmlChar*)"des");
                        memcpy(name.name, (char*)attr, strlen((char*)attr));
                    }
                    else
                    {
                        ERROR_LOG("file %s not include level element", xml_file);
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    g_female_name_vec.push_back(name);
                }//endif
                child_node = child_node->next->next;
            }//while
        }
        cur_node = cur_node->next->next;
    }//endwhile

    xmlFreeDoc(doc);

    g_male_name_num = g_male_name_vec.size();
    g_female_name_num = g_female_name_vec.size();

    DEBUG_LOG("男孩名字：%d个， 女孩名字:%d个", g_male_name_num, g_female_name_num);

    return 0;
}


int parse_xml_activity()
{
    g_activity_map.clear();
    const char *xml_file = config_get_strval("activity_file");
    if(NULL == xml_file)
    {
        ERROR_LOG("xml load task_file failed.");
        return -1;
    }

    xmlDocPtr doc = xmlParseFile(xml_file);
    if(NULL == doc)
    {
        ERROR_LOG("parse xml file failed(%s).", strerror(errno));
        return -1;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(doc);
    if(cur_node == NULL)
    {
        ERROR_LOG("Get root node of file %s failed(%s).", xml_file, strerror(errno));
        xmlFreeDoc(doc);
        return -1;
    }

    if(xmlStrcmp(cur_node->name, BAD_CAST"activities"))
    {
        ERROR_LOG("Root element of file %s is not activities", xml_file);
        xmlFreeDoc(doc);
        return -1;
    }

    xmlNodePtr child_node;
    cur_node = cur_node->xmlChildrenNode;
    cur_node = cur_node->next;
    while(cur_node != NULL)
    {
        xmlChar *attr = NULL;
        uint32_t activity_id = 0;
        activity_info_t a_activity;
        if(!xmlStrcmp(cur_node->name, BAD_CAST "activity"))
        {
            if(xmlHasProp(cur_node, (const xmlChar*)"id"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"id");
                activity_id = atoi((char*)attr);
            }
            else
            {
                ERROR_LOG("file %s not include id element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }

            a_activity.activity_id = activity_id;

            if(xmlHasProp(cur_node, (const xmlChar*)"begin_time"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"begin_time");
                a_activity.start_timestamp = get_utc_second_ex((char*)attr);
                if(a_activity.start_timestamp == 0 || a_activity.start_timestamp == (uint32_t)-1)
                {
                    ERROR_LOG("file %s begin-time %s format error", xml_file, (char*)attr);
                    xmlFreeDoc(doc);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("file %s not include begin_time element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }


            if(xmlHasProp(cur_node, (const xmlChar*)"end_time"))
            {
                attr = xmlGetProp(cur_node, (const xmlChar*)"end_time");
                a_activity.end_timestamp = get_utc_second_ex((char*)attr);
                if(a_activity.end_timestamp == 0 || a_activity.end_timestamp == (uint32_t)-1)
                {
                    ERROR_LOG("file %s end_time %s format error", xml_file, (char*)attr);
                    xmlFreeDoc(doc);
                    return -1;
                }
            }
            else
            {
                ERROR_LOG("file %s not include end_time element", xml_file);
                xmlFreeDoc(doc);
                return -1;
            }



            child_node = cur_node->xmlChildrenNode;
            child_node = child_node->next;
            while(child_node != NULL)
            {
                xmlChar *child_attr = NULL;
                task_reward_t task_reward;
                if(!xmlStrcmp(child_node->name, BAD_CAST "reward"))
                {
                    if(xmlHasProp(child_node, (const xmlChar*)"id"))
                    {
                        child_attr = xmlGetProp(child_node, (const xmlChar*)"id");
                        task_reward.reward_id = atoi((char*)child_attr);
                    }
                    else
                    {
                        ERROR_LOG("file %s not include id element", xml_file);
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    if(xmlHasProp(child_node, (const xmlChar*)"num"))
                    {
                        child_attr = xmlGetProp(child_node, (const xmlChar*)"num");
                        task_reward.reward_num = atoi((char*)child_attr);
                    }
                    else
                    {
                        ERROR_LOG("file %s not include num element", xml_file);
                        xmlFreeDoc(doc);
                        return -1;
                    }

                    a_activity.reward_vec.push_back(task_reward);
                }
                child_node = child_node->next->next;
            }

            DEBUG_LOG("a_activity_reward_num:%u", a_activity.reward_vec.size());
            g_activity_map.insert(pair<uint32_t, activity_info_t>(activity_id, a_activity));
        }
        cur_node = cur_node->next->next;
    }

    xmlFreeDoc(doc);

    return 0;
}


int parse_xml_data()
{
    const char *allow_ip = config_get_strval("allowed_ip");
    if(NULL == allow_ip)
    {
        ERROR_LOG("no allow_ip conf.");
        return -1;
    }

    strncpy(g_allowed_ip, allow_ip, 15);
    DEBUG_LOG("allow ip %s", g_allowed_ip);

    DEBUG_LOG("in parse xml data");
    //解析item表
    if (parse_xml_items() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse xml item");
    g_conf_parse_func[ITEMS_CONF] = parse_xml_items;

    if(parse_xml_shops() != 0)
    {
        return -1;
    }
     DEBUG_LOG("parse xml shops");
    g_conf_parse_func[SHOPS_CONF] = parse_xml_shops;

    //解析小怪兽xml表
    if (parse_xml_pet() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse xml pet");
    g_conf_parse_func[PET_CONF]    = parse_xml_pet;

    //解析等级对应经验的xml表
    if (parse_xml_level() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse xml level");
    g_conf_parse_func[LEVEL_CONF] = parse_xml_level;

    //解析商店各种稀有度物品的数量
    if (parse_xml_game() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse xml game");
    g_conf_parse_func[GAME_CONF]   = parse_xml_game;

    if(parse_xml_interactive() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse xml interactive");
    g_conf_parse_func[INTERACTIVE_CONF]    = parse_xml_interactive;

    //解析系统需要的各种数值
    if (parse_xml_system() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse xml system");
    g_conf_parse_func[SYSTEM_CONF] = parse_xml_system;

    if(parse_xml_badge() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse xml badge successed");
    g_conf_parse_func[BADGE_CONF] = parse_xml_badge;

    if(parse_xml_pet_series() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse xml pet_series successed");
    g_conf_parse_func[PET_SERIES_CONF] = parse_xml_pet_series;

    if(parse_xml_formulaes() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse xml formulaes successed");
    g_conf_parse_func[FORMULAS_CONF] = parse_xml_formulaes;

    if(parse_xml_bobo() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse xml bobo successed");
    g_conf_parse_func[BOBO_CONF] = parse_xml_bobo;

    if(parse_xml_maintain() != 0)
    {
        return -1;
    }
    DEBUG_LOG("pasrs maintain successed");
    g_conf_parse_func[MAINTAIN_CONF]   = parse_xml_maintain;

    if(parse_xml_game_change() != 0)
    {
        return -1;
    }
    DEBUG_LOG("pasrs game_change successed");
    g_conf_parse_func[GAME_CHANGE_CONF] = parse_xml_game_change;

    if(parse_xml_task() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse task successed");
    g_conf_parse_func[TASK_CONF] = parse_xml_task;

    if(parse_xml_donate() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse donate successed");
    g_conf_parse_func[DENOTE_CONF] = parse_xml_donate;

    if(parse_xml_museum() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse museum successed");
    g_conf_parse_func[MUSEUM_CONF] = parse_xml_museum;

    if(parse_xml_user_name() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse user name set successsed");
    g_conf_parse_func[USER_NAME_CONF] = parse_xml_user_name;

    if(parse_xml_activity() != 0)
    {
        return -1;
    }
    DEBUG_LOG("parse activity  successsed");
    g_conf_parse_func[ACTIVITY_CONF] = parse_xml_activity;


    //加入版署米米号
    g_depversion_id_set.insert((uint32_t)359539737);
    g_depversion_id_set.insert((uint32_t)359521811);
    g_depversion_id_set.insert((uint32_t)358408019);
    g_depversion_id_set.insert((uint32_t)359664142);
    g_depversion_id_set.insert((uint32_t)356671421);
    g_depversion_id_set.insert((uint32_t)356800664);


    return 0;
}
