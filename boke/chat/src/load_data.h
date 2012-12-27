/*
 * =========================================================================
 *
 *        Filename: load_data.h
 *
 *        Version:  1.0
 *        Created:  2011-05-18 13:09:57
 *        Description:  
 *
 *        Author:  ian (ian@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#ifndef  LOAD_DATA_H
#define  LOAD_DATA_H

#include <libxml/tree.h>
#define DECODE_XML_PROP(cur, prop, str) \
    str = xmlGetProp(cur, (xmlChar*)prop);  \
if (!str) ERROR_RETURN (("xml parse error: prop=%s", prop), -1)

#define DECODE_XML_PROP_DEFAULT(cur, prop, str ) \
    str = xmlGetProp(cur, (xmlChar*)prop);  



#define DECODE_XML_PROP_INT(v, cur, prop) \
        do { \
            xmlChar *str; \
            DECODE_XML_PROP(cur, prop, str); \
            v = atoi ((const char*)str); \
            xmlFree (str); \
        } while (0)

#define DECODE_XML_PROP_INT_DEFAULT(v, cur, prop,def) \
        do { \
            xmlChar *str; \
            DECODE_XML_PROP(cur, prop, str); \
			if (str){\
	            v = atoi ((const char*)str); \
	            xmlFree (str); \
			}else{\
            	v = def; \
			}\
        } while (0)


int load_xmlconf(const char* file, int (*parser)(xmlNodePtr cur_node));

int load_game_svr_config(xmlNodePtr cur);

#endif  /*LOAD_DATA_H*/
