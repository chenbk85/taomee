
#ifndef PICSRV_H
#define PICSRV_H

#include <libxml/tree.h>

#include "benchapi.h"

#define PIC_SRV_MAX_NUM		128
#define PIC_SRV_CONF		"./conf/picsrv.xml"
#define TUYA_SRV_MAX_NUM	128
#define TUYA_SRV_CONF		"./conf/tuyasrv.xml"


typedef struct ip_port{
	char		ip[16];
	in_port_t	port;
	uint32_t    ip_int;
}__attribute__ ((packed))ip_port_t;

int load_picsrv_config(xmlNodePtr cur_node);
int get_pic_srv_ip_port_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int load_tuyasrv_config(xmlNodePtr cur_node);
int get_tuya_srv_ip_port_cmd(sprite_t* p, const uint8_t* body, int bodylen);


#endif
