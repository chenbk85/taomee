#include <cstring>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <map>
#include "attr_config_data.hpp"
#include "pea_utils.hpp"

extern "C"
{
#include <glib.h>
#include <assert.h>
#include <libtaomee/tlog/tlog.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
}

using namespace taomee;
using namespace std;



// 共用的字段可以用这个parse
// 有不同的字段，自己再手动写代码解析
int parse_attr_config_data(xmlNodePtr node, attr_config_data * data)
{
    if (NULL == node || NULL == data)
    {
        return -1;
    }

    get_xml_prop_def(data->id_, node, "id", 0);
    get_xml_prop_def(data->hp_, node, "hp", 0);
    get_xml_prop_def(data->exp_, node, "exp", 0);
    get_xml_prop_def(data->level_, node, "level", 0);
    get_xml_prop_def(data->magic_, node, "magic", 0);
    get_xml_prop_def(data->agility_, node, "agility", 0);
    get_xml_prop_def(data->luck_, node, "luck", 0);
    get_xml_prop_def(data->anger_, node, "anger", 0);
    get_xml_prop_def(data->strength_, node, "strength", 0);
    get_xml_prop_def(data->atk_, node, "atk", 0);
    get_xml_prop_def(data->def_, node, "def", 0);
    get_xml_prop_def(data->happy_, node, "happy", 0);
    get_xml_prop_def(data->soul_, node, "soul", 0);
    get_xml_prop_def(data->intensity_, node, "intensity", 0);
    get_xml_prop_def(data->crit_, node, "crit", 0);
    get_xml_prop_def(data->double_hit_, node, "double_hit", 0);
    get_xml_prop_def(data->speed_, node, "speed", 0);
    get_xml_prop_def(data->crit_damage_, node, "crit_damage", 0);
    get_xml_prop_def(data->tenacity_, node, "tenacity", 0);
    get_xml_prop_def(data->physique_, node, "physique", 0);
    return 0;
}

int init_player_attr_config_data(xmlNodePtr root)
{

    xmlNodePtr attr_node = root->xmlChildrenNode;
    while(attr_node)
    {
        if (is_xmlnode_name(attr_node, "attribute"))
        {
            attr_config_data *p_data = new attr_config_data();

            parse_attr_config_data(attr_node, p_data);

            uint32_t key = p_data->level_;
            if (attr_config_data_mgr::instance().has_object(key))
            {
                throw XmlParseError(string("level data = %u has exist", key));
                return -1;
            }
            attr_config_data_mgr::instance().insert_object(key, p_data);
        }


        attr_node = attr_node->next;
    }


    return 0;
}



