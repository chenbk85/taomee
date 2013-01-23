#include "pea_utils.hpp"





bool is_xmlnode_name(xmlNodePtr node, const char * name)
{
    if (NULL == node || NULL == name)
    {
        return false;
    }
    return (0 == xmlStrcmp(node->name, BAD_CAST(name)));
}


