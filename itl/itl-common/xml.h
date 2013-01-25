/** 
 * ========================================================================
 * @file xml.h
 * @brief 
 * @author TAOMEE
 * @version 1.0
 * @date 2012-11-06
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_XML_H_2012_11_06
#define H_XML_H_2012_11_06

#include <libxml/tree.h>
#include <libtaomee++/conf_parser/xmlparser.hpp>


inline bool is_xmlnode_name(xmlNodePtr node, const char * name)
{
    if (NULL == node || NULL == name)
    {
        return false;
    }
    return (0 == xmlStrcmp(node->name, BAD_CAST(name)));
}

template <size_t len>
inline int get_xml_prop_string(char (&val)[len], xmlNodePtr cur, const void * prop)
{
    try
    {
        taomee::get_xml_prop_raw_str(val, cur, prop);
    }
    catch (taomee::XmlParseError & e)
    {
        return -1;
    }
    return 0;
}


template <typename T1>
int get_xml_prop(T1& val, xmlNodePtr cur, const void * prop)
{
    try
    {
        taomee::get_xml_prop(val, cur, prop);
    }
    catch (taomee::XmlParseError & e)
    {
        return -1;
    }

    return 0;
}

#define xmlnode_for_each(parent, child)    \
    for (xmlNodePtr child = (parent)->xmlChildrenNode; child != NULL; child = child->next)



#endif
