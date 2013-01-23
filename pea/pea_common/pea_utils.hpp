#ifndef H_UTILS_H_2012_05_02
#define H_UTILS_H_2012_05_02


#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <libtaomee++/inet/pdumanip.hpp>


inline void pack32(void * pkg, uint32_t value, int & idx)
{
    taomee::pack(pkg, value, idx);
}

inline void pack16(void * pkg, uint16_t value, int & idx)
{
    taomee::pack(pkg, value, idx);
}


inline void pack8(void * pkg, uint8_t value, int & idx)
{
    taomee::pack(pkg, value, idx);
}


inline void pack32_h(void * pkg, uint32_t value, int & idx)
{
    taomee::pack_h(pkg, value, idx);
}

inline void pack16_h(void * pkg, uint16_t value, int & idx)
{
    taomee::pack_h(pkg, value, idx);
}


inline void pack8_h(void * pkg, uint8_t value, int & idx)
{
    taomee::pack_h(pkg, value, idx);
}


bool is_xmlnode_name(xmlNodePtr node, const char * name);


#define xmlnode_for_each(parent, child)    \
    for (xmlNodePtr child = (parent)->xmlChildrenNode; child != NULL; child = child->next)

#endif
