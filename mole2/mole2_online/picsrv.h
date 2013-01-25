
#ifndef PICSRV_H
#define PICSRV_H

#include <libxml/tree.h>

#include "benchapi.h"

#define PIC_SRV_CONF		"./conf/picsrv.xml"

int load_picsrv_config(xmlNodePtr cur_node);
int get_pic_srv_ip_port_cmd(sprite_t *p, uint8_t *body, uint32_t len);


#endif
