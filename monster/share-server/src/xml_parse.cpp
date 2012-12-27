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
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <errno.h>

extern "C" 
{
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
}

#include "data.h"

using std::map;
using std::set;
using std::pair;
using std::vector;

static int parse_xml_system()
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

        if (!xmlStrcmp(cur_node->name, BAD_CAST "health_desc_offline"))
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

        cur_node = cur_node->next->next;
    }

    return 0;
}

int parse_xml_data()
{
    //解析系统需要的各种数值
    if (parse_xml_system() != 0)
    {
        return -1;
    }

    return 0;
}

