#ifndef		UPDATE_MMAP_H
#define		UPDATE_MMAP_H

#include <stdint.h>

typedef enum status{
	idc_normal_ip_recv_ok		=1,
	idc_link_ip_recv_ok			=1,
}status_e;

extern "C" int update_mmap(uint16_t proto, uint32_t packet_tag, uint32_t offset);

#endif
